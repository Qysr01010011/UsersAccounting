//
// Created by fsi on 24.11.2025.
//

#include "../../headers/ui/NewServerUi.h"
#include <qwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qformlayout.h>
#include <qboxlayout.h>
#include <qvalidator.h>
#include <qregularexpression.h>

void Ui::NewServerUi::setupUi(QWidget *form) {verticalLayout = new QVBoxLayout(form);
    horizontalLayout = new QHBoxLayout();
    formLayout = new QFormLayout();

    verticalLayout->addLayout(formLayout);
    verticalLayout->addLayout(horizontalLayout);

    leName = new QLineEdit(form);
    leName->setObjectName("ServerName");
    leName->setFocus(Qt::ActiveWindowFocusReason);

    leIp = new QLineEdit(form);
    leIp->setObjectName("IpAddress");
    leIp->setFocus(Qt::TabFocusReason);
    leIp->setMaxLength(15);

    lePort = new QLineEdit(form);
    lePort->setObjectName("Port");
    lePort->setFocus(Qt::TabFocusReason);
    lePort->setValidator(new QIntValidator(0, 99999, lePort));

    pbApply = new QPushButton("Ок", form);
    pbApply->setObjectName("Apply");
    pbApply->setFocus(Qt::TabFocusReason);

    pbCancel = new QPushButton("Отмена", form);
    pbCancel->setObjectName("Cancel");
    pbCancel->setFocus(Qt::TabFocusReason);

    formLayout->addRow("Название сервера:", leName);
    formLayout->addRow("IP адрес:", leIp);
    formLayout->addRow("Порт:", lePort);

    horizontalLayout->addWidget(pbApply);
    horizontalLayout->addWidget(pbCancel);
}