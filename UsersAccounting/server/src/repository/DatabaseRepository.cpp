//
// Created by fsi on 18.11.2025.
//

#include "repository/DatabaseRepository.h"
#include <iostream>
#include <filesystem>


DatabaseRepository::DatabaseRepository():
    m_dbPath(getDatabasePath()),
    m_dbExists(std::filesystem::exists(m_dbPath)),
    m_finish(false) {

    if(checkAndCreateDatabaseDir()) {

        if (sqlite3_config(SQLITE_CONFIG_MULTITHREAD) != SQLITE_OK) {
            std::lock_guard lock(m_io_mtx);
            std::cerr << "Can't set SQLITE_CONFIG_MULTITHREAD\n";
        } else {
            std::cout << "SQLite configured: MULTITHREAD\n";
        }


        if (!m_dbExists)
            if (!createAndInitDatabase())
                throw std::runtime_error("Can't create DB!!!");

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
        std::cerr << "Error of creating database!!!" << std::endl << "All requests to database will be ignored!" << std::endl;
}

DatabaseRepository::~DatabaseRepository() {
    m_finish = true;
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
            std::cerr << "Request database error: " << sqlite3_errmsg(db) << std::endl;
        else
            std::cerr << "Request database error: db pointer is invalid!" << std::endl;

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


bool DatabaseRepository::checkAndCreateDatabaseDir() {
    std::cout << "Check database path: " << m_dbPath << std::endl;
    std::filesystem::path dbPath = m_dbPath;

    if(!std::filesystem::exists(dbPath)) {
        std::cout << m_dbPath << " does not exists! Try to create..." << std::endl;

        std::error_code error;
        std::filesystem::create_directories(dbPath.parent_path(), error);

        if(error) {
            std::cerr << "Error of creating database path: " << error.value() << ", message: " << error.message() << std::endl;
            return false;
        }

        std::cout << "Database path successfully created!" << std::endl;
    } else
        std::cout << m_dbPath << " is exists!" << std::endl;

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
            std::cerr << "Creating table error: " << err << std::endl;
            sqlite3_free(err);
            sqlite3_close(db);
            return false;
        } else
            std::cout << "Table created successfully!" << std::endl;


        char* errmsg = nullptr;
        sqlite3_exec(db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errmsg);
        if (errmsg) {
            std::lock_guard lock(m_io_mtx);
            std::cerr << "PRAGMA journal_mode=WAL error: " << errmsg << std::endl;
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
        std::cerr << (m_dbExists ? "Opening" : "Creating") << " database error: " << sqlite3_errmsg(*db) << std::endl;
        sqlite3_close(*db);
    } else {
        std::cout << "Database was " << (m_dbExists ? "opened" : "created") << " successfully!" << std::endl;
        sqlite3_busy_timeout(*db, 5000);
    }

    return dbIsValid;
}


void DatabaseRepository::handleData(Json::Value &&newData, std::function<void(Json::Value &&)> && callback) {
    std::cout << "handleData: " << newData.toStyledString() << std::endl;
    if(!m_dbExists) {
        handleError("No database connection!!!", std::move(callback));
        return;
    }

    if(!newData.isMember("action")) {
        Json::Value response;
        response["status"] = "error";
        response["data"] = "'Action' field is required! Unknown action!";
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
        handleError("Ошибка формирования запроса!", std::move(callback));

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
                    handleError("Ошибка сохранения данных из БД!", std::move(callback));
            } else
                handleError("Ошибка формирования запроса!", std::move(callback));
        } else
            handleError("Ошибка формирования запроса!", std::move(callback));
    } else
        handleError("Ошибка формирования запроса!", std::move(callback));

    sqlite3_finalize(row);
}


void DatabaseRepository::handleDelete(Json::Value &&deleteData, sqlite3 *db, std::function<void(Json::Value &&)> && callback) {
    Json::Value response;
    int id = deleteData["id"].asInt();
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
                handleError("Ошибка удаления данных из БД!", std::move(callback));
        } else
            handleError("Ошибка формирования запроса!", std::move(callback));
    } else
        handleError("Ошибка формирования запроса!", std::move(callback));

    sqlite3_finalize(row);
}


void DatabaseRepository::handleError(std::string &&errMessage, std::function<void(Json::Value &&)> &&callback) {
    Json::Value error;
    error["status"] = "error";
    error["data"] = errMessage;
    callback(std::move(error));
}


void DatabaseRepository::startModifier() {
    sqlite3 *db = nullptr;
    bool dbOpened = openDatabase(&db, false);

    while(true) {
        if(!dbOpened) {
            std::lock_guard lock(m_io_mtx);
            std::cerr << "Can't open Database in read/write mode!";
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if(m_finish)
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
            std::cerr << "Can't open Database in read mode!";
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
