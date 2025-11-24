//
// Created by fsi on 14.11.2025.
//

#include "../../headers/ui/MainUi.h"
#include <qwidget.h>
#include <qboxlayout.h>
#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qheaderview.h>
#include <qcombobox.h>
#include <qlistview.h>


void Ui::MainUi::setupUi(QWidget *form) {
    verticalLayout = new QVBoxLayout(form);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout2 = new QHBoxLayout();

    tblUsers = new QTableWidget(0, 2, form);
    tblUsers->setObjectName("UsersTable");
    tblUsers->setHorizontalHeaderLabels({"Имя пользователя", "Электронная почта"});
    tblUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
    tblUsers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tblUsers->horizontalHeader()->setStretchLastSection(true);

    pbAddServer = new QPushButton("Добавить сервер", form);
    pbAddServer->setObjectName("AddServer");

    pbAddUser = new QPushButton("Добавить пользователя", form);
    pbAddUser->setObjectName("AddUser");

    pbDeleteUser = new QPushButton("Удалить пользователя", form);
    pbDeleteUser->setObjectName("DeleteUser");
    pbDeleteUser->setDisabled(true);

    cbbServers = new QComboBox(form);
    cbbServers->setObjectName("ServersList");
    cbbServers->setMaxVisibleItems(10);

    horizontalLayout->addWidget(cbbServers);
    horizontalLayout->addWidget(pbAddServer);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(tblUsers);
    verticalLayout->addLayout(horizontalLayout2);
    horizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred));
    horizontalLayout2->addWidget(pbDeleteUser);
    horizontalLayout2->addWidget(pbAddUser);
}
