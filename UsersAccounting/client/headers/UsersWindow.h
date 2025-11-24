//
// Created by fsi on 14.11.2025.
//

#pragma once


#include <qwidget.h>
#include "models/ServerData.h"

namespace Ui {
    class MainUi;
}

class UsersWindow: public QWidget {
    Q_OBJECT

    Ui::MainUi* m_ui = nullptr;
    QVector<ServerData> m_serversList;

    void showEvent(QShowEvent *event) override;
public:
    explicit UsersWindow(QWidget* parent = nullptr);
    ~UsersWindow() override;

    void resizeAndMove();

private:
    void createConnections();
    void loadServersFromConfig();
    void writeServersToConfig();
    void addNewUser(int id, const QString& userName, const QString& email);

private slots:
    void handleAddNewUserClicked();
    void handleAddNewServerClicked();
    void handleDeleteUserClicked();
    void handleTableItemClicked();
    void handleServerSelected(int item);
    void handleServerConnectionResponse(const QJsonObject& data);
    void handleNewUserAdded(const QJsonObject& data);
    void handleNewServerCreated(const QString& name, const QString& ip, int port);
    void handleUserDeleted(const QJsonObject& data);
    void handleShowUsers(const QJsonArray& data);
};
