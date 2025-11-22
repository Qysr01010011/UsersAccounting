//
// Created by fsi on 16.11.2025.
//

#pragma once

#include <qjsonobject.h>

class ServerConnection;

class UsersViewModel: public QObject {
    Q_OBJECT
    ServerConnection* m_connection = nullptr;

    explicit UsersViewModel();
    ~UsersViewModel();
    inline static UsersViewModel* m_instance = nullptr;

    void setConnections();
public:

    inline static UsersViewModel* getInstance() {
        if(m_instance == nullptr)
            m_instance = new UsersViewModel();

        return m_instance;
    }

    void addNewUser(const QString& userName, const QString& email);
    void deleteUser(int userId);
    void getUsersList();

signals:
    void connectedToServer(const QJsonObject&);
    void newUserAdded(const QJsonObject&);
    void userDeleted(const QJsonObject&);
    void showUsers(const QJsonArray&);
};
