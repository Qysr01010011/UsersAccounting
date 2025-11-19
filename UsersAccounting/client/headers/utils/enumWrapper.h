//
// Created by x01010011 on 19.11.2025.
//

#pragma once

#include <string>
#include <algorithm>
#include "enums.h"

void toLowercaseString(std::string& str) {
    std::for_each(str.begin(), str.end(), [](char& ch){
        ch = static_cast<char>(std::tolower(ch));
    });
}

namespace enums::wrap {
    std::string toString(Action val) {
        switch (val) {
            case Action::SELECT:
                return "select";
            case Action::INSERT:
                return "insert";
            case Action::DELETE:
                return "delete";
            case Action::STATUS:
                return "status";
            default: return "unknown";
        }
    }

    Action fromString(std::string&& str) {
        toLowercaseString(str);
        Action action = Action::UNKNOWN;

        if(str == "select")
            action = Action::SELECT;
        else if(str == "insert")
            action = Action::INSERT;
        else if(str == "delete")
            action = Action::DELETE;
        else if(str == "status")
            action = Action::STATUS;

        return action;
    }
}
