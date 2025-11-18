//
// Created by fsi on 18.11.2025.
//

#pragma once

#include <sqlite3.h>
#include <string>


class DatabaseRepository {
    constexpr bool getUserDir(const char* env, std::string& dir) {
        dir = env ? std::string(env) : "";

        return env && (env[0] != '\0');
    }

    std::string getDatabasePath() {
        std::string userDir;

#ifdef _WIN32
        std::string userDatabaseDir = "\\UsersAccounting\\server\\database\\usersDatabase.sqlite";

        if(getUserDir(std::getenv("APPDATA"), userDir))
            return userDir + "\\" + userDatabaseDir;

        if(getUserDir(std::getenv("USERPROFILE"), userDir))
            return std::string(env) + "\\AppData\\Roaming\\" + userDatabaseDir;

        return ".\\" + userDatabaseDir;
#else
        std::string userDatabaseDir = ".local/share/UsersAccounting/server/database/usersDatabase.sqlite";

        if(getUserDir(std::getenv("XDG_DATA_HOME"), userDir))
            return userDir + "/" + userDatabaseDir;

        if(getUserDir(std::getenv("HOME"), userDir))
            return userDir + "/" + userDatabaseDir;

        return "~/" + userDatabaseDir;
#endif
    }

    bool m_dbExists;
    bool dbFileExists();
    bool createAndInitDatabase();
    bool openDatabase();

    sqlite3* m_db;

public:
    explicit DatabaseRepository();
    ~DatabaseRepository();

    //TODO define and implement insert and select functions
};
