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


    namespace fromstr {
        int fromString(std::string &&str) {
            toLowercaseString(str);
            int result = -1;

            if (str == "select")
                result = static_cast<int>(Action::SELECT);
            else if (str == "insert")
                result = static_cast<int>(Action::INSERT);
            else if (str == "delete")
                result = static_cast<int>(Action::DELETE);
            else if (str == "connected")
                result = static_cast<int>(Status::CONNECTED);
            else if (str == "successful")
                result = static_cast<int>(Status::SUCCESSFUL);
            else if (str == "error")
                result = static_cast<int>(Status::ERROR);

            return static_cast<int>(result);
        }
    }


    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, std::string>
    constexpr toString(T val) {
        return tostr::toString(val);
    }

    template<typename T>
    std::enable_if_t<std::is_enum_v<T>, T>
    constexpr fromString(std::string&& str) {
        toLowercaseString(str);
        int retval = fromstr::fromString(std::move(str));
        return static_cast<T>(retval);
    }
}
