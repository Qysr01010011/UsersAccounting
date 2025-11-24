#include <drogon/drogon.h>
#include "controllers/ServerManager.h"
#include "models/CommandLineData.h"
#include "cmd_parser/CommandLineParser.h"

#ifdef _WIN32
#include <windows.h>
SetConsoleOutputCP(CP_UTF8)
#endif

int main(int argc, char* argv[]) {
    //Set HTTP listener address and port
    std::cout << "Hello, Drogon!" << std::endl;

    CommandLineData cld = CommandLineParser::parse(argc, argv);

    if(cld.isHelp) {
        CommandLineParser::printHelp();
        return 0;
    }

    drogon::app().addListener("0.0.0.0", 8080, false);
    drogon::app().setLogPath("logs");
    drogon::app().setLogLevel(trantor::Logger::kInfo);

    std::cout << "Start server!" << std::endl;

    drogon::app().run();

    return 0;
}
