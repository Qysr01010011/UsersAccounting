//
// Created by fsi on 24.11.2025.
//

#pragma once


class QWidget;
class QLineEdit;
class QPushButton;
class QFormLayout;
class QHBoxLayout;
class QVBoxLayout;

namespace Ui {
    class NewServerUi {
    public:
        QLineEdit* leName;
        QLineEdit* leIp;
        QLineEdit* lePort;
        QPushButton* pbApply;
        QPushButton* pbCancel;
        QFormLayout* formLayout;
        QHBoxLayout* horizontalLayout;
        QVBoxLayout* verticalLayout;

        void setupUi(QWidget* form);
    };
}
