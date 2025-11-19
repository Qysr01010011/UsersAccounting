#pragma once

#include <drogon/WebSocketController.h>
#include "repository/DatabaseRepository.h"

using namespace drogon;

namespace controllers
{

class ServerManager : public drogon::WebSocketController<ServerManager>
{
    std::atomic<int> m_connectionId;
    std::unique_ptr<DatabaseRepository> m_dbRep;
    std::vector<WebSocketConnectionPtr> m_connections;
    std::mutex m_mtx;

  public:
    explicit ServerManager();
    void handleNewMessage(const WebSocketConnectionPtr&,
                                  std::string &&,
                                  const WebSocketMessageType &) override;
    void handleNewConnection(const HttpRequestPtr &,
                                     const WebSocketConnectionPtr&) override;
    void handleConnectionClosed(const WebSocketConnectionPtr&) override;

    bool isValid(const Json::Value& data);

    WS_PATH_LIST_BEGIN
     WS_PATH_ADD("/api/users");
    WS_PATH_LIST_END
};
}
