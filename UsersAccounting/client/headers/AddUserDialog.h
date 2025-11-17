//
// Created by fsi on 14.11.2025.
//

#pragma once

#include <qdialog.h>

class QTextEdit;

namespace Ui {
    class NewUserUi;
}

class AddUserDialog: public QDialog {
    Q_OBJECT

    Ui::NewUserUi* m_ui = nullptr;

    void showEvent(QShowEvent* e) override;
public:
    explicit AddUserDialog(QWidget* parent = nullptr);
    ~AddUserDialog() override;

public slots:
    void handleApplyClicked();

private:
    void createConnections();
    void resizeAndMove();
    void setTextEditError(QTextEdit* te, bool isError);
};
