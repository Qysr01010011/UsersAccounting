#include <drogon/drogon.h>
#include "controllers/ServerManager.h"

int main() {
    //Set HTTP listener address and port
    std::cout << "Hello, Drogon!" << std::endl;
    std::cout << "Set ip: 0.0.0.0 and port: 8080" << std::endl;
    drogon::app().addListener("0.0.0.0", 8080, false);
    std::cout << "Start server!" << std::endl;
    drogon::app().run();

    return 0;
}
