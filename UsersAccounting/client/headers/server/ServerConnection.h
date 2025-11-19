//
// Created by fsi on 16.11.2025.
//

#pragma once

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qwebsocket.h>
#include <qjsonvalue.h>


class ServerConnection: public QObject {
    Q_OBJECT
    QWebSocket m_ws;

private slots:
    void setConnections();
    void onConnected();
    void onDisconnected();
    void onReceivedMessage(const QString& message);
    void onError(QAbstractSocket::SocketError error);
    void sendMessage(QJsonObject&& data);
public:
    explicit ServerConnection();
    ~ServerConnection();

    void addUser(const QJsonObject& data);
    void deleteUser(const QJsonObject& data);

    QJsonArray getUsersList();
};
