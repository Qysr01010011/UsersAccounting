//
// Created by fsi on 14.11.2025.
//

#include "../../headers/ui/NewUserUi.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qlineedit.h>


void Ui::NewUserUi::setupUi(QWidget *form) {
    verticalLayout = new QVBoxLayout(form);
    horizontalLayout = new QHBoxLayout();
    formLayout = new QFormLayout();

    verticalLayout->addLayout(formLayout);
    verticalLayout->addLayout(horizontalLayout);

    leUserName = new QLineEdit(form);
    leUserName->setObjectName("UserName");
    leUserName->setFocus(Qt::ActiveWindowFocusReason);

    leEmail = new QLineEdit(form);
    leEmail->setObjectName("Email");
    leEmail->setFocus(Qt::TabFocusReason);

    pbApply = new QPushButton("Ок", form);
    pbApply->setObjectName("Apply");
    pbApply->setFocus(Qt::TabFocusReason);

    pbCancel = new QPushButton("Отмена", form);
    pbCancel->setObjectName("Cancel");
    pbCancel->setFocus(Qt::TabFocusReason);

    formLayout->addRow("Имя пользователя:", leUserName);
    formLayout->addRow("Электронная почта:", leEmail);

    horizontalLayout->addWidget(pbApply);
    horizontalLayout->addWidget(pbCancel);
}
