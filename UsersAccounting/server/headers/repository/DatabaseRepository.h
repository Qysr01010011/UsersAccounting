//
// Created by fsi on 16.11.2025.
//

#pragma once

#include <sqlite3.h>

class DatabaseRepository {
    sqlite3_file* m_db = nullptr;
public:
    explicit DatabaseRepository();
    ~DatabaseRepository();
};
