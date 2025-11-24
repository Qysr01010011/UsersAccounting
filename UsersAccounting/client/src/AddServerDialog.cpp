//
// Created by fsi on 14.11.2025.
//

#include "../headers/AddServerDialog.h"
#include "../headers/ui/NewServerUi.h"
#include "../headers/view/UsersViewModel.h"

#include <qguiapplication.h>
#include <qscreen.h>
#include <qpushbutton.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlineedit.h>
#include <iostream>
#include <regex>

AddServerDialog::AddServerDialog(QWidget *parent): QDialog(parent),
   m_ui(new Ui::NewServerUi){
    m_ui->setupUi(this);
    this->setWindowTitle("Добавить сервер");
    this->setWindowModality(Qt::WindowModal);

    createConnections();
}


AddServerDialog::~AddServerDialog() {
    delete m_ui;
    std::cout << "AddServerDialog destroyed" << std::endl;
}


void AddServerDialog::createConnections() {
    connect(m_ui->pbApply, &QPushButton::clicked, this, &AddServerDialog::handleApplyClicked);
    connect(m_ui->pbCancel, &QPushButton::clicked, this, &AddServerDialog::close);
}


void AddServerDialog::resizeAndMove() {
    this->setFixedSize(400, 140);

    QPoint center;

    if(this->parent() != nullptr)
        center = this->parentWidget()->geometry().center();
    else {
        QScreen* screen = this->screen();
        center = screen->availableGeometry().center();
    }

    this->move(center.x() - this->width() / 2, center.y() - this->height() / 2);
}


void AddServerDialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);

    resizeAndMove();
}


void AddServerDialog::handleApplyClicked() {
    bool nameIsValid = !m_ui->leName->text().trimmed().isEmpty();

    setTextEditError(m_ui->leName, !nameIsValid);

    if(nameIsValid) {
        emit serverAdded(m_ui->leName->text(), m_ui->leIp->text(), m_ui->lePort->text().toInt());
        this->close();
    }
}


void AddServerDialog::setTextEditError(QLineEdit *te, bool isError) {
    QPalette pal = te->palette();
    if(isError)
        pal.setColor(QPalette::Normal, QPalette::Base, QColor::fromRgb(215, 95, 95));
    else
        pal.setColor(QPalette::Normal, QPalette::Base, this->palette().color(QPalette::Base));
    te->setPalette(pal);
}
