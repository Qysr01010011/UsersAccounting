//
// Created by fsi on 14.11.2025.
//

#pragma once


class QWidget;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QPushButton;

namespace Ui {
    class NewUserUi {
    public:
        QVBoxLayout *verticalLayout;
        QHBoxLayout *horizontalLayout;
        QFormLayout *formLayout;
        QLineEdit *leUserName;
        QLineEdit *leEmail;
        QPushButton *pbApply;
        QPushButton *pbCancel;

        void setupUi(QWidget *form);
    };
}
