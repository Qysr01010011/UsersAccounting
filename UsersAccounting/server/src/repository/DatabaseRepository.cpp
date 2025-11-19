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

    if(!dbIsValid) {
        std::cerr << (m_dbExists ? "Opening" : "Creating") << " database error: " << sqlite3_errmsg(*db) << std::endl;
        sqlite3_close(*db);
    } else
        std::cout << "Database was " << (m_dbExists ? "opened" : "created") << " successfully!" << std::endl;

    return dbIsValid;
}


void DatabaseRepository::handleData(Json::Value &&newData, std::function<void(Json::Value &&)> && callback) {
    if(!newData.isMember("action")) {
        Json::Value response;
        response["status"] = "'Action' field is required! Unknown action!";
        callback(std::move(response));
        return;
    }

    std::string action = newData["action"].asString();

    if(action == "select") {
        std::lock_guard lock(m_readMtx);
        m_readQueue.emplace(std::move(newData), callback);
        m_readCV.notify_one();
    } else {
        std::lock_guard lock(m_modifyMtx);
        m_modifyQueue.emplace(std::move(newData), callback);
        m_modifyCV.notify_one();
    }
}


void DatabaseRepository::handleSelectAll(Json::Value &&selectData, std::function<void(Json::Value &&)> && callback) {
    sqlite3* db;

    if(openDatabase(&db)) {
        sqlite3_stmt * row;

        Json::Value response;
        response["list"] = Json::arrayValue;

        if(check(sqlite3_prepare_v2(db, "SELECT * FROM users;", -1, &row, nullptr), SQLITE_OK, db))
            while(sqlite3_step(row) == SQLITE_ROW) {
                Json::Value user;
                user["id"] = sqlite3_column_int(row, 0);
                user["userName"] = reinterpret_cast<const char*>(sqlite3_column_text(row, 1));
                user["email"] = reinterpret_cast<const char*>(sqlite3_column_text(row, 2));
                response["list"].append(user);
            }

        callback(std::move(response));
        sqlite3_finalize(row);
    }
}


void DatabaseRepository::handleInsert(Json::Value &&insertData, std::function<void(Json::Value &&)> && callback) {
    sqlite3* db;

    if(openDatabase(&db)) {
        Json::Value response;
        sqlite3_stmt* row;

        int result = sqlite3_prepare_v2(db, "INSERT INTO users(userName, email) VALUES(?,?)", -1, &row, nullptr);

        if(check(result, SQLITE_OK, db)) {
            result = sqlite3_bind_text(row, 1, insertData["userName"].asString().c_str(), -1, SQLITE_TRANSIENT);

            if(check(result, SQLITE_OK, db)) {
                result = sqlite3_bind_text(row, 2, insertData["email"].asString().c_str(), -1, SQLITE_TRANSIENT);

                if(check(result, SQLITE_OK, db)) {
                    if (check(sqlite3_step(row), SQLITE_DONE, db)) {
                        response["status"] = "success";
                        response["response"]["id"] = static_cast<int>(sqlite3_last_insert_rowid(db));
                        response["response"]["userName"] = insertData["userName"].asString();
                        response["response"]["email"] = insertData["email"].asString();
                        callback(std::move(response));
                    }
                }
            }
        }

        sqlite3_finalize(row);
    }
}


void DatabaseRepository::handleDelete(Json::Value &&deleteData, std::function<void(Json::Value &&)> && callback) {
    sqlite3* db;

    if(openDatabase(&db)) {
        Json::Value response;
        int id = deleteData["id"].asInt();
        sqlite3_stmt* row;
        int result = sqlite3_prepare_v2(db, "DELETE FROM users WHERE id=?;", -1, &row, nullptr);

        if(check(result, SQLITE_OK, db)) {
            result = sqlite3_bind_int(row, 1, id);

            if(check(result, SQLITE_OK, db)) {
                if (check(sqlite3_step(row), SQLITE_DONE, db)) {
                    response["status"] = "success";
                    response["response"]["id"] = id;
                    callback(std::move(response));
                }
            }
        }

        sqlite3_finalize(row);
    }
}


void DatabaseRepository::startModifier() {
    while(!m_finish) {
        std::unique_lock lock(m_modifyMtx);
        m_modifyCV.wait(lock, [this]() { return !m_modifyQueue.empty() || m_finish; });

        if (m_finish)
            return;

        std::pair<Json::Value, std::function<void(Json::Value &&)> > data = std::move(m_modifyQueue.front());
        m_modifyQueue.pop();

        std::string action = data.first["action"].asString();

        if (action == "insert")
            handleInsert(std::move(data.first), std::move(data.second));
        else if (action == "delete")
            handleDelete(std::move(data.first), std::move(data.second));
    }
}


void DatabaseRepository::startReader() {
    while(!m_finish) {
        std::unique_lock lock(m_readMtx);
        m_readCV.wait(lock, [this](){return !m_readQueue.empty() || m_finish;});

        if(m_finish)
            return;

        std::pair<Json::Value, std::function<void(Json::Value &&)> > data;

        data = std::move(m_readQueue.front());
        m_readQueue.pop();
        lock.unlock();

        handleSelectAll(std::move(data.first), std::move(data.second));
    }
}
