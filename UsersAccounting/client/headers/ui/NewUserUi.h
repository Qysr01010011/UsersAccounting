//
// Created by fsi on 14.11.2025.
//

#pragma once


class QWidget;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QPushButton;

namespace Ui {
    class NewUserUi {
    public:
        QVBoxLayout *m_verticalLayout;
        QHBoxLayout *m_horizontalLayout;
        QFormLayout *m_formLayout;
        QTextEdit *m_teUserName;
        QTextEdit *m_teEmail;
        QPushButton *m_pbApply;
        QPushButton *m_pbCancel;

        void setupUi(QWidget *form);
    };
}
