//
// Created by saveliy on 24.11.2025.
//

#include "../../headers/cmd_parser/CommandLineParser.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <trantor/utils/Logger.h>


bool CommandLineParser::validIpAddress(const std::string &ip) {
    std::vector<std::string> ipOctats;
    std::stringstream ss(ip);
    std::string octet;
    int octatCount = 0;

    while(getline(ss, octet, '.')){
        if(octet.empty() || (octet.length() > 3)) {
            LOG_WARN << "Invalid ip address! Use default!";
            return false;
        }

        for(char c: octet)
            if(!std::isdigit(c)) {
                LOG_WARN << "Invalid ip address! Use default!";
                return false;
            }

        int octetNum = std::stoi(octet);

        if((octetNum < 0) || (octetNum > 255)) {
            LOG_WARN << "Invalid ip address! Use default!";
            return false;
        }

        ++octatCount;
    }

    return octatCount == 4;
}


bool CommandLineParser::validPort(const std::string &port) {
    bool valid = std::regex_match(port, std::regex("^\\d{1,5}$"));

    if(!valid)
        LOG_WARN << "Port is invalid! Use default!";

    return valid;
}


CommandLineData CommandLineParser::parse(int argc, char **argv) {
    CommandLineData data;

    if(argc == 1) {
        data.ip = "0.0.0.0";
        data.port = 8080;
    } else
        for(int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);

            if((arg == "--help") || (arg == "-h"))
                data.isHelp = true;
            else if((arg == "--ip") || (arg == "-i")) {
                if(++i < argc)
                    data.ip = validIpAddress(std::string(argv[i])) ? std::string(argv[i]) : "0.0.0.0";
            } else if((arg == "--port") || (arg == "-p"))
                if(++i < argc)
                    data.port = validPort(std::string(argv[i])) ? std::stoi(argv[i]) : 8080;
        }

    return data;
}


void CommandLineParser::printHelp() {
    LOG_INFO << R"(
---------------------------------------- HELP ----------------------------------------
    --ip    | -i: ip-address for listen. [0.0.0.0 by default]
    --port  | -p: port for listen. [8080 by default]
    --help  | -h: server info.

Examples:
    start server with listen of default ip-address and port
        ./users-accounting-server

    start server with listen of 192.168.123.43 ip and 1234 port for example
        ./users-accounting-server -i 192.168.123.43 -p 1234
        ./users-accounting-server --ip 192.168.123.43 --port 1234

    start server with listen of ip only
        ./users-accounting-server -i 192.168.123.43
        ./users-accounting-server --ip 192.168.123.43

    start server with listen of port only
        ./users-accounting-server  -p 1234
        ./users-accounting-server  --port 1234

    show help for server
        ./users-accounting-server -h
        ./users-accounting-server --help)";
}