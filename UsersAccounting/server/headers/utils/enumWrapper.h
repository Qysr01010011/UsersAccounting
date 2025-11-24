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
    namespace tostr {
        std::string toString(Action val) {
            switch (val) {
                case Action::SELECT:
                    return "select";
                case Action::INSERT:
                    return "insert";
                case Action::DELETE:
                    return "delete";
                default:
                    return "unknown";
            }
        }

        std::string toString(Status val) {
            switch (val) {
                case Status::CONNECTED:
                    return "connected";
                case Status::SUCCESSFUL:
                    return "successful";
                case Status::ERROR:
                    return "error";
                default:
                    return "unknown";
            }
        }
    }


    namespace action {
        Action fromString(const std::string &str) {
            std::string actionStr(str);
            toLowercaseString(actionStr);

            Action action = Action::UNKNOWN;

            if (str == "select")
                action = Action::SELECT;
            else if (str == "insert")
                action = Action::INSERT;
            else if (str == "delete")
                action = Action::DELETE;

            return action;
        }
    }


    namespace status {
        Status fromString(const std::string &str) {
            std::string statusStr(str);
            toLowercaseString(statusStr);

            Status status = Status::UNKNOWN;

            if (str == "connected")
                status = Status::CONNECTED;
            else if (str == "successful")
                status = Status::SUCCESSFUL;
            else if (str == "error")
                status = Status::ERROR;

            return status;
        }
    }


    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, std::string>
    constexpr toString(T val) {
        return tostr::toString(val);
    }
}
