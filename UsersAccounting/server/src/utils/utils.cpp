//
// Created by fsi on 17.11.2025.
//

#include "utils/utils.h"


std::string utils::getUserDatabaseDir() {
    const char* env;

#ifdef _WIN32
    env = std::getenv("APPDATA");

    if(env && env[0] != '\0')
        return std::string(env) + "\\UserAccounting\\server\\database";

    env = std::getenv("USERPROFILE");

    if(env && env[0] != '\0')
        return std::string (env) + "\\AppData\\Roaming\\UserAccounting\\server\\database";

    return ".\\UserAccounting\\server\\database";
#else
    env = std::getenv("XDG_DATA_HOME");

    if(env && env[0] != '\0')
        return std::string(env) + "/.local/share/UserAccounting/server/database";

    env = std::getenv("HOME");

    if(env && env[0] != '\0')
        return std::string(env) + "/.local/share/UserAccounting/server/database";

    return "~/.local/share/UserAccounting/server/database";
#endif
}


std::string utils::getSystemDatabaseDir() {
#ifdef _WIN32
    return "C:\\ProgramData\\UserAccounting\\server\\database";
#else
    return "/usr/local/UserAccounting/server/database";
#endif
}


bool utils::checkDatabaseInUserDir() {
    return false;
}


bool utils::copyDatabaseToUserDir() {
    return false;
}
