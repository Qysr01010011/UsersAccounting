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
}


UsersWindow::~UsersWindow() {
    delete m_ui;
}


void UsersWindow::createConnections() {
    connect(m_ui->m_pbAddUser, &QPushButton::clicked, this, &UsersWindow::handleAddNewUserClicked);
    connect(UsersViewModel::getInstance(), &UsersViewModel::newUserAdded, this, &UsersWindow::handleAddNewUser);
    connect(UsersViewModel::getInstance(), &UsersViewModel::showUsers, this, &UsersWindow::handleShowUsers);
}


void UsersWindow::handleAddNewUserClicked() {
    AddUserDialog* dlg = new AddUserDialog(this);
    dlg->exec();
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


void UsersWindow::handleAddNewUser(const QJsonObject &data) {
    addNewUser(1, data["userName"].toString(), data["email"].toString());
}


void UsersWindow::handleShowUsers(const QJsonArray &data) {
    for(const QJsonValue& val: data) {
        if(val.isObject()) {
            QJsonObject obj = val.toObject();
            addNewUser(1, obj["userName"].toString(), obj["email"].toString());
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
