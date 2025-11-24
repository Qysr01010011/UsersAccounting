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
#include <qlineedit.h>
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
    connect(m_ui->pbApply, &QPushButton::clicked, this, &AddUserDialog::handleApplyClicked);
    connect(m_ui->pbCancel, &QPushButton::clicked, this, &AddUserDialog::close);
}


void AddUserDialog::resizeAndMove() {
    this->setFixedSize(400, 120);

    QPoint center;

    if(this->parent() != nullptr)
        center = this->parentWidget()->geometry().center();
    else {
        QScreen* screen = this->screen();
        center = screen->availableGeometry().center();
    }

    this->move(center.x() - this->width() / 2, center.y() - this->height() / 2);
}


void AddUserDialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);

    resizeAndMove();
}


void AddUserDialog::handleApplyClicked() {
    QRegularExpression re("^[a-z0-9\\.]+@[a-z]+\\.[a-z]{2,3}$");
    std::string mail = m_ui->leEmail->text().toStdString();

    bool userNameIsValid = !m_ui->leUserName->text().trimmed().isEmpty(),
         emailIsValid = re.match(m_ui->leEmail->text()).hasMatch();

    setTextEditError(m_ui->leUserName, !userNameIsValid);
    setTextEditError(m_ui->leEmail, !emailIsValid);

    if(userNameIsValid && emailIsValid) {
        UsersViewModel::getInstance()->addNewUser(m_ui->leUserName->text(), m_ui->leEmail->text());
        this->close();
    }
}


void AddUserDialog::setTextEditError(QLineEdit *le, bool isError) {
    QPalette pal = le->palette();

    if(isError)
        pal.setColor(QPalette::Normal, QPalette::Base, QColor::fromRgb(215, 95, 95));
    else
        pal.setColor(QPalette::Normal, QPalette::Base, this->palette().color(QPalette::Base));
    le->setPalette(pal);
}
