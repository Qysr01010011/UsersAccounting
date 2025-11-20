#include <drogon/drogon.h>
#include "controllers/ServerManager.h"

int main() {
    //Set HTTP listener address and port

    drogon::app()
    .addListener("0.0.0.0", 8080)
    .run();

    return 0;
}
