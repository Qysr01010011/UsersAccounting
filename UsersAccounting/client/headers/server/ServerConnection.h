//
// Created by fsi on 16.11.2025.
//

#pragma once

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>


class ServerConnection {
public:
    explicit ServerConnection();
    ~ServerConnection();

    void addUser(const QJsonObject& data);

    QJsonArray getUsersList();
};
