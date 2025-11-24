//
// Created by fsi on 14.11.2025.
//

#pragma once

class QWidget;
class QTableWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QComboBox;

namespace Ui {
    class MainUi {
    public:
        QVBoxLayout *verticalLayout;
        QHBoxLayout *horizontalLayout;
        QHBoxLayout *horizontalLayout2;
        QTableWidget *tblUsers;
        QPushButton *pbAddUser;
        QPushButton *pbDeleteUser;
        QPushButton *pbAddServer;
        QComboBox* cbbServers;

        void setupUi(QWidget *form);
    };
}
