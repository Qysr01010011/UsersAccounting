//
// Created by fsi on 16.11.2025.
//

#include "view/UsersViewModel.h"
#include "server/ServerConnection.h"


UsersViewModel::UsersViewModel():
        m_connection(new ServerConnection){
    setConnections();
}


UsersViewModel::~UsersViewModel() {
    delete m_connection;
}


void UsersViewModel::setConnections() {
    connect(m_connection, &ServerConnection::connectionResponse, this, &UsersViewModel::connectedToServer);
    connect(m_connection, &ServerConnection::selectResponse, this, &UsersViewModel::showUsers);
    connect(m_connection, &ServerConnection::insertResponse, this, &UsersViewModel::newUserAdded);
    connect(m_connection, &ServerConnection::deleteResponse, this, &UsersViewModel::userDeleted);
}


void UsersViewModel::addNewUser(const QString &userName, const QString &email) {
    QJsonObject newUserData;

    newUserData["userName"] = QJsonValue(userName);
    newUserData["email"] = QJsonValue(email);

    m_connection->requestForAddUser(std::move(newUserData));
}


void UsersViewModel::deleteUser(int userId) {
    QJsonObject deleteUserData;
    deleteUserData["id"] = QJsonValue(userId);

    m_connection->requestForDeleteUser(std::move(deleteUserData));
}


void UsersViewModel::getUsersList() {
    m_connection->requestForUsersList();
}
