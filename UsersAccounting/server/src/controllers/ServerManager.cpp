#include "controllers/ServerManager.h"

using namespace controllers;


ServerManager::ServerManager(): drogon::WebSocketController<controllers::ServerManager>(),
        m_dbRep(std::make_unique<DatabaseRepository>()){

}


void ServerManager::handleNewMessage(const WebSocketConnectionPtr& wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    //TODO write your application logic here
}


void ServerManager::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr& wsConnPtr)
{
    //TODO write your application logic here
}


void ServerManager::handleConnectionClosed(const WebSocketConnectionPtr& wsConnPtr)
{
    //TODO write your application logic here
}
