#pragma once

#include <drogon/WebSocketController.h>

using namespace drogon;

namespace controllers
{
class ServerManager : public drogon::WebSocketController<ServerManager>
{
  public:
     void handleNewMessage(const WebSocketConnectionPtr&,
                                  std::string &&,
                                  const WebSocketMessageType &) override;
    void handleNewConnection(const HttpRequestPtr &,
                                     const WebSocketConnectionPtr&) override;
    void handleConnectionClosed(const WebSocketConnectionPtr&) override;
    WS_PATH_LIST_BEGIN
     WS_PATH_ADD("/api/users");
    WS_PATH_LIST_END
};
}
