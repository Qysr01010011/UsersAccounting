//
// Created by saveliy on 24.11.2025.
//

#include "../../headers/cmd_parser/CommandLineParser.h"
#include <iostream>
#include <regex>
#include <sstream>


bool CommandLineParser::validIpAddress(const std::string &ip) {
    std::vector<std::string> ipOctats;
    std::stringstream ss(ip);
    std::string octat;
    int octatCount = 0;

    while(getline(ss, octat, '.')){
        if(std::regex_match(octat, std::regex("\\d{1-3}"))) {
            int octatNum = std::stoi(octat);
            if ((octatNum <= 255) && (octatNum >= 0))
                ++octatCount;
            else {
                std::cerr << "Invalid ip address! Set default 0.0.0.0!";
                return false;
            }
        } else {
            std::cerr << "Invalid ip address! Set default 0.0.0.0!";
            return false;
        }
    }

    return octatCount == 4;
}


bool CommandLineParser::validPort(const std::string &port) {
    bool valid = std::regex_match(port, std::regex("\\d{1-5}"));

    if(!valid)
        std::cerr << "Port is invalid! Set default 8080!"

    return valid;
}


CommandLineData CommandLineParser::parse(int argc, char **argv) {
    CommandLineData data;

    for(int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if((arg == "--help") || (arg == "-h"))
            data.isHelp = true;
        else if((arg == "--ip") || (arg == "-i"))
            data.ip = validIpAddress(std::string(argv[++i])) ? std::string(argv[i]) : "0.0.0.0";
        else if((arg == "--port") || (arg == "-p"))
            data.port = validPort(std::string(argv[++i])) ? std::stoi(argv[i]) : 8080;
    }

    return data;
}


void CommandLineParser::printHelp() {
    std::cout << "-------- HELP --------" << std::endl
              << "--help | -h: server info." << std::endl
              << "--ip <address> | -i <address>: ip-address for listen. [0.0.0.0 by default]" << std::endl
              << "--port <port> | -p <port>: port for listen. [8080 by default]" << std::endl
              << "Examples:" << std::endl
              << "start server with listen of default ip-address and port" << std::endl
              << "\t./users-accounting-server" << std::endl
              << "start server with listen of 192.168.123.43 ip and 1234 port for example" << std::endl
              << "\t./users-accounting-server -i 192.168.123.43 -p 1234" << std::endl
              << "\t./users-accounting-server --ip 192.168.123.43 --port 1234" << std::endl
              << "show help for server" << std::endl
              << "\t./users-accounting-server -h" << std::endl
              << "\t./users-accounting-server --help" << std::endl;
}