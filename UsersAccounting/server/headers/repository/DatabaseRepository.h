//
// Created by fsi on 18.11.2025.
//

#pragma once

#include <sqlite3.h>
#include <string>
#include <condition_variable>
#include <queue>
#include <json/value.h>
#include <functional>
#include <filesystem>


class DatabaseRepository {
    bool getUserDir(const char *env, std::string &dir) {
        if (!env || env[0] == '\0')
            return false;

        dir = env;
        return true;
    }

    std::string getDatabasePath() {
        namespace fs = std::filesystem;
        fs::path base;
        const std::string relDbPath = "UserAccounting/server/database/usersDatabase.sqlite";

#ifdef _WIN32
        base = std::getenv("APPDATA") ? std::getenv("APPDATA") : fs::path(std::getenv("USERPROFILE")) / "AppData/Roaming";
#elif defined(APPLE)
        base = std::filesystem::path(std::getenv("HOME")) / "Library/Application Support";
#else
        base = std::getenv("XDG_DATA_HOME") ? fs::path(std::getenv("XDG_DATA_HOME")) : fs::path(std::getenv("HOME")) / ".local/share";
#endif

        return (base / relDbPath).string();
    }

    sqlite3 *m_dbModifier = nullptr;
    const std::size_t READERS_COUNT = std::max(2u, (std::thread::hardware_concurrency() * 4u) - 1u);
    const std::string m_dbPath;
    bool m_dbExists;
    std::atomic<bool> m_finish;

    std::thread m_modifier;
    std::vector<std::thread> m_readers;

    std::queue<std::pair<Json::Value, std::function<void(Json::Value &&)> > > m_modifyQueue,
            m_readQueue;

    std::condition_variable m_readCV,
                            m_modifyCV;

    std::mutex m_modifyMtx, m_readMtx, m_io_mtx;

public:
    explicit DatabaseRepository();

    ~DatabaseRepository();

    void handleData(Json::Value &&newData, std::function<void(Json::Value &&)> && callback);

private:
    bool dbFileExists();

    bool checkAndCreateDatabaseDir();

    bool check(int result, int target, sqlite3 *db);

    bool createAndInitDatabase();

    bool openDatabase(sqlite3 **db, bool isReadOnly);

    void handleSelectAll(Json::Value &&selectData, sqlite3 *db, std::function<void(Json::Value &&)> && callback);

    void handleInsert(Json::Value &&insertData, sqlite3 *db, std::function<void(Json::Value &&)> && callback);

    void handleDelete(Json::Value &&deleteData, sqlite3 *db, std::function<void(Json::Value &&)> && callback);

    void handleError(std::string&& errMessage, std::string&& action, std::function<void(Json::Value&&)> && callback);

    void startModifier();

    void startReader();
};
