//
// Created by fsi on 14.11.2025.
//

#pragma once

#include <qdialog.h>

class QLineEdit;

namespace Ui {
    class NewServerUi;
}

class AddServerDialog: public QDialog {
    Q_OBJECT

    Ui::NewServerUi* m_ui = nullptr;

    void showEvent(QShowEvent* e) override;
public:
    explicit AddServerDialog(QWidget* parent = nullptr);
    ~AddServerDialog() override;

signals:
    void serverAdded(const QString&, const QString, int);

private slots:
    void handleApplyClicked();

private:
    void createConnections();
    void resizeAndMove();
    void setTextEditError(QLineEdit* te, bool isError);
};
