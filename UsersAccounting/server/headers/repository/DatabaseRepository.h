//
// Created by fsi on 18.11.2025.
//

#pragma once

#include <sqlite3.h>
#include <string>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include <queue>


class DatabaseRepository {
    constexpr bool getUserDir(const char *env, std::string &dir) {
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

        if (getUserDir(std::getenv("XDG_DATA_HOME"), userDir))
            return userDir + "/" + userDatabaseDir;

        if (getUserDir(std::getenv("HOME"), userDir))
            return userDir + "/" + userDatabaseDir;

        return "~/" + userDatabaseDir;
#endif
    }

    const std::size_t READERS_COUNT = std::max(1u, std::thread::hardware_concurrency() * 4) - 1;
    const std::string m_dbPath;
    bool m_dbExists;
    bool m_finish;

    std::thread m_modifier;
    std::vector<std::thread> m_readers;

    std::queue<std::pair<nlohmann::json, void(*)(nlohmann::json&&)> > m_modifyQueue,
            m_readQueue;

    std::condition_variable m_readCV,
                            m_modifyCV;

    std::mutex m_mtx, m_io_mtx, m_task_mtx;

public:
    explicit DatabaseRepository();

    ~DatabaseRepository();

    void handleData(nlohmann::json &&newData, void(*callback)(nlohmann::json &&));

private:
    bool dbFileExists();

    bool check(int result, int target, sqlite3 *db);

    bool createAndInitDatabase();

    bool openDatabase(sqlite3 **db);

    void handleSelectAll(nlohmann::json &&selectData, void(*callback)(nlohmann::json &&));

    void handleInsert(nlohmann::json &&insertData, void(*callback)(nlohmann::json &&));

    void handleDelete(nlohmann::json &&deleteData, void(*callback)(nlohmann::json &&));

    void startModifier();

    void startReader();
};
