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

namespace Ui {
    class MainUi {
    public:
        QVBoxLayout *m_verticalLayout;
        QHBoxLayout *m_horizontalLayout;
        QTableWidget *m_tblUsers;
        QPushButton *m_pbAddUser;
        QPushButton *m_pbDeleteUser;
        QSpacerItem *m_spacer;

        void setupUi(QWidget *form);
    };
}
