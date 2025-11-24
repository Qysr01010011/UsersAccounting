//
// Created by saveliy on 24.11.2025.
//

#pragma once
#include <vector>
#include <string>
#include "../models/CommandLineData.h"


class CommandLineParser {
    bool static validIpAddress(const std::string& ip);
    bool static validPort(const std::string& port);
public:
    CommandLineData static parse(int argc, char* argv[]);

    void static printHelp();
};
