//
// Created by fsi on 18.11.2025.
//

#include "repository/DatabaseRepository.h"
#include <iostream>
#include <filesystem>
#include <trantor/utils/Logger.h>


DatabaseRepository::DatabaseRepository():
    m_dbPath(getDatabasePath()),
    m_dbExists(std::filesystem::exists(m_dbPath)),
    m_finish(false) {

    if(checkAndCreateDatabaseDir()) {
        int rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);

        if (rc != SQLITE_OK && rc != SQLITE_MISUSE) {
            std::lock_guard lock(m_io_mtx);
            LOG_WARN << "Can't set SQLITE_CONFIG_MULTITHREAD\n";
        } else {
            LOG_INFO << "SQLite configured: MULTITHREAD\n";
        }


        if (!m_dbExists)
            if (!createAndInitDatabase()) {
                LOG_FATAL << "Can't create DB!!!";
                return;
            }

        if (m_dbExists) {
            sqlite3 *tmp = nullptr;

            if (openDatabase(&tmp, true)) {
                char *errMsg = nullptr;
                sqlite3_exec(tmp, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errMsg);

                if (errMsg)
                    sqlite3_free(errMsg);

                sqlite3_close(tmp);
            }

            m_modifier = std::thread(&DatabaseRepository::startModifier, this);

            m_readers.reserve(READERS_COUNT);

            while (m_readers.size() < READERS_COUNT)
                m_readers.emplace_back(&DatabaseRepository::startReader, this);
        }
    } else
        LOG_ERROR << "Error of creating database!!!\n" << "All requests to database will be ignored!";
}

DatabaseRepository::~DatabaseRepository() {
    m_finish.store(true, std::memory_order_release);
    m_readCV.notify_all();
    m_modifyCV.notify_all();

    for(std::thread& reader: m_readers)
        reader.join();

    m_modifier.join();
}


bool DatabaseRepository::check(int result, int target, sqlite3* db) {
    if(result != target) {
        std::lock_guard lock(m_io_mtx);

        if(db)
            LOG_ERROR << "Request database error: " << sqlite3_errmsg(db);
        else
            LOG_ERROR << "Request database error: db pointer is invalid!";

        return false;
    }

    return true;
}


bool DatabaseRepository::dbFileExists() {
    if(!(m_dbExists = std::filesystem::exists(getDatabasePath()))) {
        LOG_ERROR << "Database does not exists. Try to create database...";
        return createAndInitDatabase();
    }

    return true;
}


bool DatabaseRepository::checkAndCreateDatabaseDir() {
    LOG_INFO << "Check database path: " << m_dbPath;
    std::filesystem::path dbPath = m_dbPath;

    if(!std::filesystem::exists(dbPath)) {
        LOG_INFO << m_dbPath << " does not exists! Try to create...";

        std::error_code error;
        std::filesystem::create_directories(dbPath.parent_path(), error);

        if(error) {
            LOG_FATAL << "Error of creating database path: " << error.value() << ", message: " << error.message();
            return false;
        }

        LOG_INFO << "Database path successfully created!";
    } else
        LOG_INFO << m_dbPath << " is exists!";

    return true;
}


bool DatabaseRepository::createAndInitDatabase() {
    sqlite3* db = nullptr;

    if(openDatabase(&db, false)) {
        char *err;

        std::string create_table_request = R"(CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            userName TEXT NOT NULL,
            email TEXT
        );)";

        int result = sqlite3_exec(db, create_table_request.c_str(), nullptr, nullptr, &err);

        if(result != SQLITE_OK) {
            LOG_FATAL << "Creating table error: " << err;
            sqlite3_free(err);
            sqlite3_close(db);
            return false;
        } else
            LOG_INFO << "Table created successfully!";


        char* errmsg = nullptr;
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errmsg);
        if (errmsg) {
            std::lock_guard lock(m_io_mtx);
            LOG_WARN << "PRAGMA journal_mode=WAL error: " << errmsg;
            sqlite3_free(errmsg);
        }

        sqlite3_busy_timeout(db, 5000);
    }

    m_dbExists = true;
    return true;
}


bool DatabaseRepository::openDatabase(sqlite3** db, bool isReadOnly) {
    int flags = isReadOnly ? (SQLITE_OPEN_READONLY) : (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

    int result = sqlite3_open_v2(getDatabasePath().c_str(), db, flags, nullptr);
    bool dbIsValid = check(result, SQLITE_OK, *db);

    if(!dbIsValid) {
        LOG_ERROR << (m_dbExists ? "Opening" : "Creating") << " database error: " << sqlite3_errmsg(*db);
        sqlite3_close(*db);
    } else {
        std::cout << "Database was " << (m_dbExists ? "opened" : "created") << " successfully!" << std::endl;
        sqlite3_busy_timeout(*db, 5000);
    }

    return dbIsValid;
}


void DatabaseRepository::handleData(Json::Value &&newData, std::function<void(Json::Value &&)> && callback) {
    LOG_INFO << "handleData: " << newData.toStyledString();
    if(!m_dbExists) {
        handleError("No database connection!!!", newData["action"].asString(), std::move(callback));
        return;
    }

    if(!newData.isMember("action")) {
        handleError("'Action' field is required! Unknown action!", newData["action"].asString(), std::move(callback));
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


void DatabaseRepository::handleSelectAll(Json::Value &&selectData, sqlite3 *db, std::function<void(Json::Value &&)> && callback) {
    sqlite3_stmt * row;

    Json::Value response;
    response["action"] = selectData["action"];
    response["status"] = "successful";
    response["data"] = Json::arrayValue;

    if(check(sqlite3_prepare_v2(db, "SELECT * FROM users;", -1, &row, nullptr), SQLITE_OK, db)) {
        while (sqlite3_step(row) == SQLITE_ROW) {
            Json::Value user;
            user["id"] = sqlite3_column_int(row, 0);
            user["userName"] = std::string(reinterpret_cast<const char *>(sqlite3_column_text(row, 1)));
            user["email"] = std::string(reinterpret_cast<const char *>(sqlite3_column_text(row, 2)));
            response["data"].append(user);
        }

        callback(std::move(response));
    } else
        handleError("Ошибка формирования запроса!", "select", std::move(callback));

    sqlite3_finalize(row);
}


void DatabaseRepository::handleInsert(Json::Value &&insertData, sqlite3 *db, std::function<void(Json::Value &&)> && callback) {
    std::cout << "handleInsert: " << insertData.toStyledString() << std::endl;
    std::string email = insertData["data"]["email"].asString();
    std::string userName = insertData["data"]["userName"].asString();

    Json::Value response;
    sqlite3_stmt* row;

    int result = sqlite3_prepare_v2(db, "INSERT INTO users(userName, email) VALUES(?,?)", -1, &row, nullptr);

    if(check(result, SQLITE_OK, db)) {
        result = sqlite3_bind_text(row, 1, userName.c_str(), -1, SQLITE_TRANSIENT);

        if(check(result, SQLITE_OK, db)) {
            result = sqlite3_bind_text(row, 2, email.c_str(), -1, SQLITE_TRANSIENT);

            if(check(result, SQLITE_OK, db)) {
                if (check(sqlite3_step(row), SQLITE_DONE, db)) {
                    std::cout << "dataWasInsert: " << insertData.toStyledString() << std::endl;
                    response["action"] = insertData["action"];
                    response["status"] = "successful";
                    response["data"]["id"] = static_cast<int>(sqlite3_last_insert_rowid(db));
                    response["data"]["userName"] = userName;
                    response["data"]["email"] = email;
                    callback(std::move(response));
                } else
                    handleError("Ошибка сохранения данных из БД!", "insert", std::move(callback));
            } else
                handleError("Ошибка формирования запроса!", "insert", std::move(callback));
        } else
            handleError("Ошибка формирования запроса!", "insert", std::move(callback));
    } else
        handleError("Ошибка формирования запроса!", "insert", std::move(callback));

    sqlite3_finalize(row);
}


void DatabaseRepository::handleDelete(Json::Value &&deleteData, sqlite3 *db, std::function<void(Json::Value &&)> && callback) {
    Json::Value response;
    int id = deleteData["data"]["id"].asInt();
    sqlite3_stmt* row;
    int result = sqlite3_prepare_v2(db, "DELETE FROM users WHERE id=?;", -1, &row, nullptr);

    if(check(result, SQLITE_OK, db)) {
        result = sqlite3_bind_int(row, 1, id);

        if(check(result, SQLITE_OK, db)) {
            if (check(sqlite3_step(row), SQLITE_DONE, db)) {
                response["action"] = deleteData["action"];
                response["status"] = "successful";
                response["data"]["id"] = id;
                callback(std::move(response));
            } else
                handleError("Ошибка удаления данных из БД!", "delete", std::move(callback));
        } else
            handleError("Ошибка формирования запроса!", "delete", std::move(callback));
    } else
        handleError("Ошибка формирования запроса!", "delete", std::move(callback));

    sqlite3_finalize(row);
}


void DatabaseRepository::handleError(std::string &&errMessage, std::string&& action, std::function<void(Json::Value &&)> &&callback) {
    Json::Value error;
    error["status"] = "error";
    error["action"] = action;
    error["data"] = errMessage;
    callback(std::move(error));
}


void DatabaseRepository::startModifier() {
    sqlite3 *db = nullptr;
    bool dbOpened = openDatabase(&db, false);

    while(true) {
        if(!dbOpened) {
            std::lock_guard lock(m_io_mtx);
            LOG_ERROR << "Can't open Database in read/write mode!";
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if(m_finish.load(std::memory_order_acquire))
                return;
            else
                continue;
        }

        std::unique_lock lock(m_modifyMtx);
        m_modifyCV.wait(lock, [this]() { return !m_modifyQueue.empty() || m_finish; });

        if (m_finish)
            break;

        std::pair<Json::Value, std::function<void(Json::Value &&)> > data = std::move(m_modifyQueue.front());
        m_modifyQueue.pop();
        lock.unlock();

        std::string action = data.first["action"].asString();

        if (action == "insert")
            handleInsert(std::move(data.first), db, std::move(data.second));
        else if (action == "delete")
            handleDelete(std::move(data.first), db, std::move(data.second));
    }
}


void DatabaseRepository::startReader() {
    sqlite3 *db = nullptr;
    bool dbOpened = openDatabase(&db, true);

    while(!m_finish) {
        if(!dbOpened) {
            std::lock_guard lock(m_io_mtx);
            LOG_ERROR << "Can't open Database in read mode!";
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if(m_finish)
                return;
            else
                continue;
        }

        std::unique_lock lock(m_readMtx);
        m_readCV.wait(lock, [this](){return !m_readQueue.empty() || m_finish;});

        if(m_finish)
            return;

        std::pair<Json::Value, std::function<void(Json::Value &&)> > data;

        data = std::move(m_readQueue.front());
        m_readQueue.pop();
        lock.unlock();

        handleSelectAll(std::move(data.first), db, std::move(data.second));
    }
}
