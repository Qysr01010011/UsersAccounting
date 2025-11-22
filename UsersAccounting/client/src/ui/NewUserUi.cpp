//
// Created by fsi on 14.11.2025.
//

#include "../../headers/ui/NewUserUi.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qtextedit.h>


void Ui::NewUserUi::setupUi(QWidget *form) {
    m_verticalLayout = new QVBoxLayout(form);
    m_horizontalLayout = new QHBoxLayout();
    m_formLayout = new QFormLayout();

    m_verticalLayout->addLayout(m_formLayout);
    m_verticalLayout->addLayout(m_horizontalLayout);

    m_teUserName = new QTextEdit(form);
    m_teUserName->setObjectName("UserName");
    m_teUserName->setTabChangesFocus(true);
    m_teUserName->setFocus(Qt::ActiveWindowFocusReason);
    m_teUserName->copyAvailable(true);
    m_teUserName->setUndoRedoEnabled(true);

    m_teEmail = new QTextEdit(form);
    m_teEmail->setObjectName("Email");
    m_teEmail->setTabChangesFocus(true);
    m_teEmail->setFocus(Qt::TabFocusReason);
    m_teEmail->copyAvailable(true);
    m_teEmail->setUndoRedoEnabled(true);

    m_pbApply = new QPushButton("Ок", form);
    m_pbApply->setObjectName("Apply");
    m_pbApply->setFocus(Qt::TabFocusReason);

    m_pbCancel = new QPushButton("Отмена", form);
    m_pbCancel->setObjectName("Cancel");
    m_pbCancel->setFocus(Qt::TabFocusReason);

    m_formLayout->addRow("Имя пользователя:", m_teUserName);
    m_formLayout->addRow("Электронная почта:", m_teEmail);

    m_horizontalLayout->addWidget(m_pbApply);
    m_horizontalLayout->addWidget(m_pbCancel);
}
