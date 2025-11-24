#include <iostream>
#include "controllers/ServerManager.h"
#include "../../headers/utils/enums.h"
#include "../../headers/utils/enumWrapper.h"

using namespace controllers;


void ServerManager::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    std::cout << "handleNewMessage: " << message << std::endl;

    if(type != WebSocketMessageType::Text)
        return;

    Json::Value data;
    Json::CharReaderBuilder builder;
    std::istringstream iss(message);
    std::string errors;

    Json::Value errResponse;
    errResponse["status"] = "ERROR";

    if(!Json::parseFromStream(builder, iss, &data, &errors)) {
        errResponse["data"] = errors;
        wsConnPtr->send(errResponse.toStyledString());
        return;
    }

    if(!isValid(data)) {
        errResponse["data"] = "Invalid request data!";
        wsConnPtr->send(errResponse.toStyledString());
        return;
    }

    m_dbRep->handleData(std::move(data), [wsConnPtr, this](Json::Value&& response){
        std::cerr << "Sending styled data: " << response.toStyledString() << std::endl;

        wsConnPtr->send(response.toStyledString());
        int currentConnectionId = *wsConnPtr->getContext<int>();

        std::string actionStr = response["action"].asString();
        std::string statusStr = response["status"].asString();
        std::string jsonStr = response.toStyledString();

        std::cout << jsonStr << std::endl;

        Action action = enums::wrap::action::fromString(actionStr);
        Status status = enums::wrap::status::fromString(statusStr);


        if((action != Action::SELECT) && (status == Status::SUCCESSFUL)) {
            std::lock_guard lock(m_mtx);
            for (const WebSocketConnectionPtr& conn: m_connections)
                if (currentConnectionId != *conn->getContext<int>()) {
                    try {
                        conn->send(response.toStyledString());
                    } catch(const std::exception& e) {
                        std::cerr << "Send error: " << e.what() << std::endl;
                    }
                }
        }
    });
}


void ServerManager::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{
    wsConnPtr->setContext(std::make_shared<int>(++m_connectionId));

    Json::Value response;
    response["status"] = "connected";
    response["id"] = m_connectionId.load(std::memory_order_acquire);

    wsConnPtr->send(response.toStyledString());

    {
        std::lock_guard lock(m_mtx);
        m_connections.push_back(wsConnPtr);
    }
}


void ServerManager::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr)
{
    std::lock_guard lock(m_mtx);
    auto rm_it = std::remove(m_connections.begin(), m_connections.end(), wsConnPtr);
    m_connections.erase(rm_it, m_connections.end());

    if(m_connections.empty())
        m_connectionId = 0;
}


bool ServerManager::isValid(const Json::Value &data) {
    if(data.isMember("action")) {
        std::string actionStr(data["action"].asCString());
        Action action = enums::wrap::action::fromString(actionStr);

        return action != Action::UNKNOWN;
    }

    return false;
}
