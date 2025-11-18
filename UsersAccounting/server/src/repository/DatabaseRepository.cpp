//
// Created by fsi on 18.11.2025.
//

#include "../../headers/DatabaseRepository.h"
#include <iostream>
#include <filesystem>


DatabaseRepository::DatabaseRepository():
    m_dbExists(std::filesystem::exists(getDatabasePath())){
    if(sqlite3_config(SQLITE_CONFIG_MULTITHREAD) != SQLITE_OK)
        std::cerr << "It is impossible set multi-thread mode in database!!!" << std::endl;

    std::cout << "Database in multi-thread mode was set!!!";

    if(!m_dbExists)
        createAndInitDatabase();
    else
        openDatabase();
}

DatabaseRepository::~DatabaseRepository() {
    if(m_db)
        sqlite3_close(m_db);
}


bool DatabaseRepository::dbFileExists() {
    if(!(m_dbExists = std::filesystem::exists(getDatabasePath()))) {
        std::cerr << "Database does not exists. Try to create database..." << std::endl;
        return createAndInitDatabase();
    }

    return true;
}


bool DatabaseRepository::createAndInitDatabase() {
    if(openDatabase()) {
        char *err;

        std::string create_table_request = R"(CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            userName TEXT NOT NULL,
            email TEXT
        );)";

        int result = sqlite3_exec(m_db, create_table_request.c_str(), nullptr, nullptr, &err);

        if(result != SQLITE_OK) {
            std::cerr << "Creating table error: " << err << std::endl;
            sqlite3_free(err);
            sqlite3_close(m_db);
            return false;
        }
    }

    m_dbExists = true;
    return true;
}


bool DatabaseRepository::openDatabase() {
    int result = sqlite3_open(getDatabasePath().c_str(), &m_db);

    if(result != SQLITE_OK) {
        std::cerr << (m_dbExists ? "Opening" : "Creating") << " database error: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    std::cout << "Database was " << (m_dbExists ? "opened" : "created") << " successfully!" << std::endl;

    return true;
}
