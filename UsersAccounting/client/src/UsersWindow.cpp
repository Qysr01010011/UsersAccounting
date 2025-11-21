//
// Created by fsi on 14.11.2025.
//

#include "../headers/UsersWindow.h"
#include "../headers/ui/MainUi.h"
#include "../headers/AddUserDialog.h"
#include "../headers/view/UsersViewModel.h"

#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qguiapplication.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qscreen.h>
#include <iostream>


UsersWindow::UsersWindow(QWidget* parent): QWidget(parent),
m_ui(new Ui::MainUi){
    m_ui->setupUi(this);
    this->setWindowTitle("Пользователи");

    createConnections();
    UsersViewModel::getInstance()->getUsersList();
}


UsersWindow::~UsersWindow() {
    delete m_ui;
}


void UsersWindow::createConnections() {
    connect(m_ui->m_pbAddUser, &QPushButton::clicked, this, &UsersWindow::handleAddNewUserClicked);
    connect(m_ui->m_pbDeleteUser, &QPushButton::clicked, this, &UsersWindow::handleDeleteUserClicked);
    connect(m_ui->m_tblUsers, &QTableWidget::itemSelectionChanged, this, &UsersWindow::handleTableItemClicked);
    connect(UsersViewModel::getInstance(), &UsersViewModel::newUserAdded, this, &UsersWindow::handleNewUserAdded);
    connect(UsersViewModel::getInstance(), &UsersViewModel::userDeleted, this, &UsersWindow::handleUserDeleted);
    connect(UsersViewModel::getInstance(), &UsersViewModel::showUsers, this, &UsersWindow::handleShowUsers);
}


void UsersWindow::handleAddNewUserClicked() {
    AddUserDialog* dlg = new AddUserDialog(this);
    dlg->exec();
}


void UsersWindow::handleDeleteUserClicked() {
    int row = m_ui->m_tblUsers->currentRow();
    int userId = m_ui->m_tblUsers->item(row, 0)->data(Qt::UserRole).toInt();

    UsersViewModel::getInstance()->deleteUser(userId);
}


void UsersWindow::handleTableItemClicked() {
    m_ui->m_pbDeleteUser->setEnabled(!m_ui->m_tblUsers->selectedItems().empty());
}


void UsersWindow::resizeAndMove() {
    this->setFixedSize(900, 600);
    QPoint center = QGuiApplication::primaryScreen()->availableGeometry().center();

    this->move(center.x() - this->width() / 2, center.y() - this->height()  / 2);
}


void UsersWindow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    resizeAndMove();
}


void UsersWindow::handleNewUserAdded(const QJsonObject &data) {
    qDebug() << "handleNewUserAdded";
    addNewUser(data["id"].toInt(), data["userName"].toString(), data["email"].toString());
}


void UsersWindow::handleUserDeleted(const QJsonObject &data) {
    qDebug() << "handleUserDeleted";
    for(int row = 0; row < m_ui->m_tblUsers->rowCount(); ++row) {
        if(m_ui->m_tblUsers->item(row, 0)->data(Qt::UserRole).toInt() == data["id"].toInt()) {
            m_ui->m_tblUsers->removeRow(row);
            m_ui->m_tblUsers->update();
            break;
        }
    }
}


void UsersWindow::handleShowUsers(const QJsonArray &data) {
    qDebug() << "handleShowUsers";
    for(const QJsonValue& val: data) {
        if(val.isObject()) {
            QJsonObject obj = val.toObject();
            addNewUser(obj["id"].toInt(), obj["userName"].toString(), obj["email"].toString());
        }
    }
}


void UsersWindow::addNewUser(int id, const QString &userName, const QString &email) {
    int currentRow = m_ui->m_tblUsers->rowCount();
    m_ui->m_tblUsers->insertRow(currentRow);

    m_ui->m_tblUsers->setItem(currentRow, 0, new QTableWidgetItem(userName));
    m_ui->m_tblUsers->setItem(currentRow, 1, new QTableWidgetItem(email));
    m_ui->m_tblUsers->item(currentRow, 0)->setData(Qt::UserRole, QVariant(id));
}
