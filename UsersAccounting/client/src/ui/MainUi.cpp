//
// Created by fsi on 14.11.2025.
//

#include "../../headers/ui/MainUi.h"
#include <qwidget.h>
#include <qboxlayout.h>
#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qheaderview.h>


void Ui::MainUi::setupUi(QWidget *form) {
    m_verticalLayout = new QVBoxLayout(form);
    m_horizontalLayout = new QHBoxLayout();

    m_tblUsers = new QTableWidget(0, 2, form);
    m_tblUsers->setObjectName("UsersTable");
    m_tblUsers->setHorizontalHeaderLabels({"Имя пользователя", "Электронная почта"});
    m_tblUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tblUsers->horizontalHeader()->setStretchLastSection(true);

    m_pbAddUser = new QPushButton("Добавить пользователя", form);
    m_pbAddUser->setObjectName("AddUser");

    m_verticalLayout->addWidget(m_tblUsers);
    m_verticalLayout->addLayout(m_horizontalLayout);
    m_horizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred));
    m_horizontalLayout->addWidget(m_pbAddUser);
}
