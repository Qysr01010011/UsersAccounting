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
    connect(m_connection, &ServerConnection::selectResponse, this, &UsersViewModel::showUsers);
    connect(m_connection, &ServerConnection::insertResponse, this, &UsersViewModel::newUserAdded);
    connect(m_connection, &ServerConnection::deleteResponse, this, &UsersViewModel::userDeleted);
}


void UsersViewModel::addNewUser(const QString &userName, const QString &email) {
    QJsonObject newUser, addRequest;

    newUser["userName"] = QJsonValue(userName);
    newUser["email"] = QJsonValue(email);

    addRequest["action"] = QJsonValue("insert");
    addRequest["data"] = QJsonValue(newUser);

    m_connection->requestForAddUser(addRequest);
}


void UsersViewModel::deleteUser(int userId) {
    QJsonObject deleteRequest;
    deleteRequest["action"] = QJsonValue("delete");
    deleteRequest["data"] = QJsonValue(userId);

    m_connection->requestForDeleteUser(deleteRequest);
}


void UsersViewModel::getUsersList() {
    m_connection->requestForUsersList();
}
