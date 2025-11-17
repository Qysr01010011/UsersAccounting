//
// Created by fsi on 17.11.2025.
//

#pragma once
#include <string>


namespace utils {
    std::string getUserDatabaseDir();

    std::string getSystemDatabaseDir();

    bool checkDatabaseInUserDir();

    bool copyDatabaseToUserDir();
}
