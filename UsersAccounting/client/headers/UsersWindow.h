//
// Created by fsi on 14.11.2025.
//

#pragma once


#include <qwidget.h>

namespace Ui {
    class MainUi;
}

class UsersWindow: public QWidget {
    Q_OBJECT

    Ui::MainUi* m_ui = nullptr;

    void showEvent(QShowEvent *event) override;
public:
    explicit UsersWindow(QWidget* parent = nullptr);
    ~UsersWindow() override;

    void resizeAndMove();

private:
    void createConnections();
    void addNewUser(int id, const QString& userName, const QString& email);

private slots:
    void handleAddNewUserClicked();
    void handleAddNewUser(const QJsonObject& data);
    void handleShowUsers(const QJsonArray& data);
};
