//
// Created by fsi on 14.11.2025.
//

#include "../headers/AddUserDialog.h"
#include "../headers/ui/NewUserUi.h"
#include "../headers/view/UsersViewModel.h"

#include <qguiapplication.h>
#include <qscreen.h>
#include <qpushbutton.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qtextedit.h>
#include <iostream>
#include <regex>

AddUserDialog::AddUserDialog(QWidget *parent): QDialog(parent),
   m_ui(new Ui::NewUserUi){
    m_ui->setupUi(this);
    this->setWindowTitle("Добавить пользователя");
    this->setWindowModality(Qt::WindowModal);

    createConnections();
}


AddUserDialog::~AddUserDialog() {
    delete m_ui;
    std::cout << "AddUserDialog destroyed" << std::endl;
}


void AddUserDialog::createConnections() {
    connect(m_ui->m_pbApply, &QPushButton::clicked, this, &AddUserDialog::handleApplyClicked);
    connect(m_ui->m_pbCancel, &QPushButton::clicked, this, &AddUserDialog::close);
}


void AddUserDialog::resizeAndMove() {
    this->setFixedSize(400, 120);

    QPoint center = mapToGlobal(QGuiApplication::primaryScreen()->availableGeometry().center());

    if(this->parent() != nullptr)
        center = mapToGlobal(this->parentWidget()->geometry().center());

    this->move(center.x() - this->width() / 2, center.y() - this->height() / 2);
}


void AddUserDialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);

    resizeAndMove();
}


void AddUserDialog::handleApplyClicked() {
    std::regex re = std::regex("^[a-z0-9\\.]+@(gmail|mail|yandex|ya|bk|inbox)\\.(ru|com)");
    std::string mail = m_ui->m_teEmail->toPlainText().toStdString();

    bool userNameIsValid = !m_ui->m_teUserName->toPlainText().trimmed().isEmpty(),
         emailIsValid = std::regex_match(m_ui->m_teEmail->toPlainText().toStdString().c_str(), std::regex(R"(^[a-zA-Z0-9.]+@(gmail|mail|yandex|ya|bk|inbox)\.(ru|com)$)"));

    setTextEditError(m_ui->m_teUserName, !userNameIsValid);
    setTextEditError(m_ui->m_teEmail, !emailIsValid);

    if(userNameIsValid && emailIsValid) {
        UsersViewModel::getInstance()->addNewUser(m_ui->m_teUserName->toPlainText(), m_ui->m_teEmail->toPlainText());
        this->close();
    }
}


void AddUserDialog::setTextEditError(QTextEdit *te, bool isError) {
    QPalette pal = te->palette();
    if(isError)
        pal.setColor(QPalette::Normal, QPalette::Base, QColor::fromRgb(215, 95, 95));
    else
        pal.setColor(QPalette::Normal, QPalette::Base, this->palette().color(QPalette::Base));
    te->setPalette(pal);
}
