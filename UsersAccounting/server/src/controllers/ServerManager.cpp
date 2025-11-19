#include "controllers/ServerManager.h"
#include "../../headers/utils/enums.h"
#include "../../headers/utils/enumWrapper.h"

using namespace controllers;


ServerManager::ServerManager():
        m_connectionId{0},
        drogon::WebSocketController<controllers::ServerManager>(),
        m_dbRep(std::make_unique<DatabaseRepository>()){

}


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
        errResponse["message"] = errors;
        wsConnPtr->sendJson(errResponse);
        return;
    }

    if(!isValid(data)) {
        errResponse["message"] = "Invalid request data!";
        wsConnPtr->sendJson(errResponse);
        return;
    }

    Action action = enums::wrap::fromString(data["action"].asString());

    m_dbRep->handleData(std::move(data), [wsConnPtr, action, this](Json::Value&& response){
        wsConnPtr->sendJson(response);
        int currentConnectionId = *wsConnPtr->getContext<int>();

        if(action != Action::SELECT) {
            response["status"] = "new";

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
    response["id"] = m_connectionId.load(std::memory_order_acq_rel);

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
        Action action = enums::wrap::fromString(data["action"].asString());

        if(action == Action::UNKNOWN)
            return false;

        return true;
    }

    return false;
}
