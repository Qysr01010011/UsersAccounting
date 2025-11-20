#include "controllers/ServerManager.h"
#include "../../headers/utils/enums.h"
#include "../../headers/utils/enumWrapper.h"

using namespace controllers;


void ServerManager::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
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
        wsConnPtr->sendJson(errResponse);
        return;
    }

    if(!isValid(data)) {
        errResponse["data"] = "Invalid request data!";
        wsConnPtr->sendJson(errResponse);
        return;
    }

    std::string actionStr = data["action"].asString();
    std::string statusStr = data["status"].asString();

    Action action = enums::wrap::fromString<Action>(std::move(actionStr));
    Status status = enums::wrap::fromString<Status>(std::move(statusStr));

    m_dbRep->handleData(std::move(data), [wsConnPtr, action, status, this](Json::Value&& response){
        wsConnPtr->sendJson(response);
        int currentConnectionId = *wsConnPtr->getContext<int>();

        if((action != Action::SELECT) && (status == Status::SUCCESSFUL)) {
            std::lock_guard lock(m_mtx);
            for (WebSocketConnectionPtr conn: m_connections)
                if (currentConnectionId != *conn->getContext<int>()) {
                    try {
                        conn->sendJson(response);
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

    wsConnPtr->sendJson(response);

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
        Action action = enums::wrap::fromString<Action>(std::move(actionStr));

        if(action == Action::UNKNOWN)
            return false;

        return true;
    }

    return false;
}
