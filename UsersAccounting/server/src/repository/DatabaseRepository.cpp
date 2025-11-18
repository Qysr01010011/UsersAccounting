//
// Created by fsi on 18.11.2025.
//

#include "repository/DatabaseRepository.h"
#include <iostream>
#include <filesystem>


DatabaseRepository::DatabaseRepository():
    m_dbPath(getDatabasePath()),
    m_dbExists(std::filesystem::exists(m_dbPath)),
    m_finish(false){
    if(sqlite3_config(SQLITE_CONFIG_MULTITHREAD) != SQLITE_OK)
        std::cerr << "It is impossible set multi-thread mode in database!!!" << std::endl;

    std::cout << "Database in multi-thread mode was set!!!";

    if(!m_dbExists)
        createAndInitDatabase();

    if(m_dbExists) {
        m_modifier = std::thread(&DatabaseRepository::startModifier, this);

        m_readers.reserve(READERS_COUNT);

        while(m_readers.size() < READERS_COUNT)
            m_readers.emplace_back(&DatabaseRepository::startReader, this);
    }
}

DatabaseRepository::~DatabaseRepository() {
    m_finish = true;
    m_readCV.notify_one();
    m_modifyCV.notify_all();

    for(std::thread& reader: m_readers)
        reader.join();

    m_modifier.join();
}


bool DatabaseRepository::check(int result, int target, sqlite3* db) {
    if(result != target) {
        std::lock_guard lock(m_io_mtx);
        std::cerr << "Request database error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}


bool DatabaseRepository::dbFileExists() {
    if(!(m_dbExists = std::filesystem::exists(getDatabasePath()))) {
        std::cerr << "Database does not exists. Try to create database..." << std::endl;
        return createAndInitDatabase();
    }

    return true;
}


bool DatabaseRepository::createAndInitDatabase() {
    sqlite3* db = nullptr;

    if(openDatabase(&db)) {
        char *err;

        std::string create_table_request = R"(CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            userName TEXT NOT NULL,
            email TEXT
        );)";

        int result = sqlite3_exec(db, create_table_request.c_str(), nullptr, nullptr, &err);

        if(result != SQLITE_OK) {
            std::cerr << "Creating table error: " << err << std::endl;
            sqlite3_free(err);
            sqlite3_close(db);
            return false;
        } else
            std::cout << "Table created successfully!" << std::endl;
    }

    m_dbExists = true;
    return true;
}


bool DatabaseRepository::openDatabase(sqlite3** db) {
    int result = sqlite3_open(getDatabasePath().c_str(), db);
    bool dbIsValid = check(result, SQLITE_OK, *db);

    if(dbIsValid)
        std::cerr << (m_dbExists ? "Opening" : "Creating") << " database error: " << sqlite3_errmsg(*db) << std::endl;
    else
        std::cout << "Database was " << (m_dbExists ? "opened" : "created") << " successfully!" << std::endl;

    return dbIsValid;
}


void DatabaseRepository::handleData(nlohmann::json &&newData, void (*callback)(nlohmann::json &&)) {
    if(!newData.contains("action")) {
        nlohmann::json response;
        response["status"] = "'Action' field is required! Unknown action!";
        callback(std::move(response));
        return;
    }

    std::string action = newData["action"];

    if(action == "select") {
        std::lock_guard lock(m_mtx);
        m_readQueue.emplace(std::move(newData), callback);
        m_readCV.notify_one();
    } else {
        std::lock_guard lock(m_mtx);
        m_modifyQueue.emplace(std::move(newData), callback);
        m_modifyCV.notify_one();
    }
}


void DatabaseRepository::handleSelectAll(nlohmann::json &&selectData, void (*callback)(nlohmann::json &&)) {
    sqlite3* db;

    if(openDatabase(&db)) {
        sqlite3_stmt * row;

        nlohmann::json response;
        response["list"] = nlohmann::json::array();

        if(check(sqlite3_prepare_v2(db, "SELECT * FROM users;", -1, &row, nullptr), SQLITE_ROW, db))
            while(sqlite3_step(row) == SQLITE_ROW) {
                response["list"].push_back({
                    {"id", sqlite3_column_int(row, 0)},
                    {"userName", reinterpret_cast<const char*>(sqlite3_column_text(row, 1))},
                    {"email", reinterpret_cast<const char*>(sqlite3_column_text(row, 2))}
                });
            }

        callback(std::move(response));
    }
}


void DatabaseRepository::handleInsert(nlohmann::json &&insertData, void (*callback)(nlohmann::json &&)) {
    sqlite3* db;

    if(openDatabase(&db)) {
        nlohmann::json response;
        std::string userName = "'+" + insertData["userName"].get<std::string>() + "+'";
        std::string email = "'+" + insertData["email"].get<std::string>() + "+'";
        std::string request("INSERT INTO users(userName, email) VALUES(" + userName + ",'" + email + "')");
        sqlite3_stmt* row;

        if(check(sqlite3_prepare_v2(db, request.c_str(), -1, &row, nullptr), SQLITE_DONE, db)) {
            sqlite3_finalize(row);
            response["status"] = "success";
            response["response"] = {{"id", 1}, {"userName", insertData[userName].get<std::string>()}, {"email", insertData["email"].get<std::string>()}};
            callback(std::move(response));
        }
    }
}


void DatabaseRepository::handleDelete(nlohmann::json &&deleteData, void (*callback)(nlohmann::json &&)) {
    sqlite3* db;

    if(openDatabase(&db)) {
        nlohmann::json response;
        int id = deleteData["id"].get<int>();
        std::string request("DELETE FROM users WHERE id=" + std::to_string(id) + ";");
        sqlite3_stmt* row;

        if(check(sqlite3_prepare_v2(db, request.c_str(), -1, &row, nullptr), SQLITE_DONE, db)) {
            sqlite3_finalize(row);
            response["status"] = "success";
            response["response"] = {{"id", deleteData}};
            callback(std::move(response));
        }
    }
}


void DatabaseRepository::startModifier() {
    while(!m_finish) {
        std::unique_lock lock(m_task_mtx);
        m_modifyCV.wait(lock, [this]() { return !m_modifyQueue.empty() || m_finish; });

        if (m_finish)
            return;

        std::pair<nlohmann::json, void(*)(nlohmann::json&&)> data = std::move(m_modifyQueue.front());
        m_modifyQueue.pop();

        std::string action = data.first["action"];

        if (action == "insert")
            handleInsert(std::move(data.first), data.second);
        else if (action == "delete")
            handleDelete(std::move(data.first), data.second);
    }
}


void DatabaseRepository::startReader() {
    while(!m_readQueue.empty()) {
        std::pair<nlohmann::json, void(*)(nlohmann::json&&)> data;

        {
            std::lock_guard lock(m_mtx);
            data = std::move(m_readQueue.front());
            m_readQueue.pop();
        }

        handleSelectAll(std::move(data.first), data.second);
    }
}
