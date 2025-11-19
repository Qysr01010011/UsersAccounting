#include <drogon/drogon.h>
int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 8080);
//    drogon::app().loadConfigFile("config.yaml");
    drogon::app().run();

    return 0;
}
