//
// Created by fsi on 16.11.2025.
//

#include "view/UsersViewModel.h"
#include "server/ServerConnection.h"


UsersViewModel::UsersViewModel():
        m_connection(new ServerConnection){
}


void UsersViewModel::addNewUser(const QJsonObject &newUser) {
    emit newUserAdded(newUser);
}


void UsersViewModel::updateUsersList() {
    emit showUsers(m_connection->getUsersList());
}
