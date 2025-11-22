//
// Created by fsi on 16.11.2025.
//

#pragma once

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qwebsocket.h>
#include <qjsonvalue.h>
#include <condition_variable>


class ServerConnection: public QObject {
    Q_OBJECT
    QWebSocket m_ws;
    bool m_isConnected = false;
    QUrl m_wsUrl = QUrl(QStringLiteral("ws://127.0.0.1:8080/api/users"));

private slots:
    void onConnected();
    void onDisconnected();
    void onReceivedMessage(const QString& message);
    void onError(QAbstractSocket::SocketError error);

signals:
    void connectionResponse(const QJsonObject&);
    void selectResponse(const QJsonArray&);
    void insertResponse(const QJsonObject&);
    void deleteResponse(const QJsonObject&);
public:
    explicit ServerConnection();
    ~ServerConnection() override;

    void requestForAddUser(QJsonObject&& data);
    void requestForDeleteUser(QJsonObject&& data);

    void requestForUsersList();

private:
    void setConnections();

    void handleJSON(QJsonObject&& obj);

    void sendMessage(QJsonObject&& data);
};
