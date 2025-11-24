//
// Created by fsi on 14.11.2025.
//

#include "../headers/UsersWindow.h"
#include "../headers/ui/MainUi.h"
#include "../headers/AddUserDialog.h"
#include "../headers/view/UsersViewModel.h"
#include "AddServerDialog.h"

#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qguiapplication.h>
#include <qsettings.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qscreen.h>
#include <iostream>


UsersWindow::UsersWindow(QWidget* parent): QWidget(parent),
m_ui(new Ui::MainUi){
    m_ui->setupUi(this);
    this->setWindowTitle("Пользователи");

    loadServersFromConfig();
    createConnections();
}


UsersWindow::~UsersWindow() {
    writeServersToConfig();
    delete m_ui;
}


void UsersWindow::createConnections() {
    connect(m_ui->pbAddUser, &QPushButton::clicked, this, &UsersWindow::handleAddNewUserClicked);
    connect(m_ui->pbAddServer, &QPushButton::clicked, this, &UsersWindow::handleAddNewServerClicked);
    connect(m_ui->pbDeleteUser, &QPushButton::clicked, this, &UsersWindow::handleDeleteUserClicked);
    connect(m_ui->tblUsers, &QTableWidget::itemSelectionChanged, this, &UsersWindow::handleTableItemClicked);
    connect(m_ui->cbbServers, QOverload<int>::of(&QComboBox::activated), this, &UsersWindow::handleServerSelected);
    connect(UsersViewModel::getInstance(), &UsersViewModel::connectedToServer, this, &UsersWindow::handleServerConnectionResponse);
    connect(UsersViewModel::getInstance(), &UsersViewModel::newUserAdded, this, &UsersWindow::handleNewUserAdded);
    connect(UsersViewModel::getInstance(), &UsersViewModel::userDeleted, this, &UsersWindow::handleUserDeleted);
    connect(UsersViewModel::getInstance(), &UsersViewModel::showUsers, this, &UsersWindow::handleShowUsers);
}


void UsersWindow::loadServersFromConfig() {
    m_serversList.push_back({"Default", "127.0.0.1", 8080, true});

    QSettings srvConfigFile("srv_config.ini", QSettings::IniFormat);
    int size = srvConfigFile.beginReadArray("servers");

    for(int i = 0; i < size; ++i) {
        srvConfigFile.setArrayIndex(i);

        m_serversList.push_back({
            srvConfigFile.value("name").toString(),
            srvConfigFile.value("ip").toString(),
            srvConfigFile.value("port").toInt(),
            srvConfigFile.value("isCurrent").toBool()
        });
    }

    srvConfigFile.endArray();

    for(const ServerData& data: m_serversList) {
        int index = m_ui->cbbServers->count();
        QString title = data.serverIp + ":" + QString::fromStdString(std::to_string(data.serverPort));

        m_ui->cbbServers->addItem(title);
        m_ui->cbbServers->setItemData(index, QVariant::fromValue(data.serverIp), Qt::UserRole);
        m_ui->cbbServers->setItemData(index, QVariant::fromValue(data.serverPort), Qt::UserRole + 1);

        if(data.isCurrent)
            m_ui->cbbServers->setCurrentIndex(index);
    }
}


void UsersWindow::writeServersToConfig() {
    QSettings srvConfigFile("srv_config.ini", QSettings::IniFormat);
    srvConfigFile.clear();
    int size = m_serversList.size();

    srvConfigFile.beginWriteArray("servers");

    for(int i = 1; i < size; ++i) {
        srvConfigFile.setArrayIndex(i);

        srvConfigFile.setValue("name", QVariant::fromValue(m_serversList.at(i).serverName));
        srvConfigFile.setValue("ip", QVariant::fromValue(m_serversList.at(i).serverIp));
        srvConfigFile.setValue("port", QVariant::fromValue(m_serversList.at(i).serverPort));
        srvConfigFile.setValue("isCurrent", QVariant::fromValue(m_serversList.at(i).isCurrent));
    }

    srvConfigFile.endArray();
}


void UsersWindow::handleAddNewUserClicked() {
    AddUserDialog dlg(this);
    dlg.exec();
}


void UsersWindow::handleAddNewServerClicked() {
    AddServerDialog dlg(this);
    connect(&dlg, &AddServerDialog::serverAdded, this, &UsersWindow::handleNewServerCreated);
    dlg.exec();
}


void UsersWindow::handleDeleteUserClicked() {
    int row = m_ui->tblUsers->currentRow();
    int userId = m_ui->tblUsers->item(row, 0)->data(Qt::UserRole).toInt();

    UsersViewModel::getInstance()->deleteUser(userId);
}


void UsersWindow::handleTableItemClicked() {
    m_ui->pbDeleteUser->setEnabled(!m_ui->tblUsers->selectedItems().empty());
}


void UsersWindow::handleServerSelected(int item) {
    QString ip = m_ui->cbbServers->itemData(item, Qt::UserRole).toString();
    int port = m_ui->cbbServers->itemData(item, Qt::UserRole + 1).toInt();

    for(ServerData& data: m_serversList)
        if(data.isCurrent){
            data.isCurrent = false;
            break;
        }

    (m_serversList.begin() + item + 1)->isCurrent = true;
    UsersViewModel::getInstance()->setNewServerData(ip, port);
}


void UsersWindow::resizeAndMove() {
    this->setFixedSize(900, 600);

    QScreen* screen = this->screen();
    QPoint center = screen->availableGeometry().center();

    this->move(center.x() - this->width() / 2, center.y() - this->height()  / 2);
}


void UsersWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    resizeAndMove();
}


void UsersWindow::handleServerConnectionResponse(const QJsonObject &data) {
    qDebug() << "handleServerConnectionResponse";
    UsersViewModel::getInstance()->getUsersList();
}


void UsersWindow::handleNewUserAdded(const QJsonObject &data) {
    qDebug() << "handleNewUserAdded";
    addNewUser(data["id"].toInt(), data["userName"].toString(), data["email"].toString());
}


void UsersWindow::handleNewServerCreated(const QString &name, const QString &ip, int port) {
    m_serversList.push_back({name, ip, port, false});

    int index = m_ui->cbbServers->count();
    m_ui->cbbServers->addItem(name);
    m_ui->cbbServers->setItemData(index, QVariant::fromValue(ip), Qt::UserRole);
    m_ui->cbbServers->setItemData(index, QVariant::fromValue(port), Qt::UserRole + 1);
}


void UsersWindow::handleUserDeleted(const QJsonObject &data) {
    qDebug() << "handleUserDeleted";
    for(int row = 0; row < m_ui->tblUsers->rowCount(); ++row) {
        if(m_ui->tblUsers->item(row, 0)->data(Qt::UserRole).toInt() == data["id"].toInt()) {
            m_ui->tblUsers->removeRow(row);
            m_ui->tblUsers->update();
            break;
        }
    }
}


void UsersWindow::handleShowUsers(const QJsonArray &data) {
    qDebug() << "handleShowUsers";
    while(m_ui->tblUsers->rowCount() > 0)
        m_ui->tblUsers->removeRow(0);
    
    for(const QJsonValue& val: data) {
        if(val.isObject()) {
            QJsonObject obj = val.toObject();
            addNewUser(obj["id"].toInt(), obj["userName"].toString(), obj["email"].toString());
        }
    }
}


void UsersWindow::addNewUser(int id, const QString &userName, const QString &email) {
    int currentRow = m_ui->tblUsers->rowCount();
    m_ui->tblUsers->insertRow(currentRow);

    m_ui->tblUsers->setItem(currentRow, 0, new QTableWidgetItem(userName));
    m_ui->tblUsers->setItem(currentRow, 1, new QTableWidgetItem(email));
    m_ui->tblUsers->item(currentRow, 0)->setData(Qt::UserRole, QVariant(id));
}
