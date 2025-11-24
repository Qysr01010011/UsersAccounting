#include <drogon/drogon.h>
#include "controllers/ServerManager.h"
#include "models/CommandLineData.h"
#include "cmd_parser/CommandLineParser.h"

#ifdef _WIN32
#include <windows.h>
#endif


int main(int argc, char* argv[]) {
    //Set HTTP listener address and port
    LOG_INFO << "Hello, Drogon!";

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8)
#endif

    CommandLineData cld = CommandLineParser::parse(argc, argv);

    if(cld.isHelp) {
        CommandLineParser::printHelp();
        return 0;
    }

    LOG_INFO << "IP = " << cld.ip << " and PORT = " << cld.port << " was set!";

    drogon::app().addListener(cld.ip, cld.port, false);
    drogon::app().setLogLevel(trantor::Logger::kInfo);

    LOG_INFO << "Start server!";

    drogon::app().run();

    return 0;
}
