//
// Created by fsi on 16.11.2025.
//

#include "server/ServerConnection.h"
#include <qjsonobject.h>
#include <qjsondocument.h>

ServerConnection::ServerConnection() {
    setConnections();
}


ServerConnection::~ServerConnection() {

}


void ServerConnection::setConnections() {
    connect(&m_ws, &QWebSocket::connected, this, &ServerConnection::onConnected);
    connect(&m_ws, &QWebSocket::disconnected, this, &ServerConnection::onDisconnected);
    connect(&m_ws, &QWebSocket::textMessageReceived, this, &ServerConnection::onReceivedMessage);
    connect(&m_ws, QOverload<QAbstractSocket::SocketError>(&QWebSocket::error), this, &ServerConnection::onError);
}


void ServerConnection::addUser(const QJsonObject &data) {
    QJsonObject request;
    request["action"] = QJsonValue("insert");
    request["user"] = data;

    sendMessage(std::move(request));
}


void ServerConnection::deleteUser(const QJsonObject &data) {
    QJsonObject request;
    request["action"] = QJsonValue("delete");
    request["user"] = data;

    sendMessage(std::move(request));
}


QJsonArray ServerConnection::getUsersList() {
    QJsonObject request;
    request["action"] = QJsonValue("select");
    return QJsonArray();
}


void ServerConnection::onConnected() {
    qDebug() << "Connected";
}


void ServerConnection::onDisconnected() {
    qCritical() << "Disconnected";
}


void ServerConnection::onReceivedMessage(const QString &message) {
    QJsonObject data;
}


void ServerConnection::onError(QAbstractSocket::SocketError error) {
    QString err_message;

    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            err_message = "Connection refused!";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            err_message = "Remote host closed error";
            break;
        case QAbstractSocket::HostNotFoundError:
            err_message = "Host not found error";
            break;
        case QAbstractSocket::SocketAccessError:
            err_message = "Socket access error";
            break;
        case QAbstractSocket::SocketResourceError:
            err_message = "Socket resource error";
            break;
        case QAbstractSocket::SocketTimeoutError:
            err_message = "Socket timeout error";
            break;
        case QAbstractSocket::DatagramTooLargeError:
            err_message = "Datagram too large error";
            break;
        case QAbstractSocket::NetworkError:
            err_message = "Network error";
            break;
        case QAbstractSocket::AddressInUseError:
            err_message = "Address in use error";
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            err_message = "Socket address not available error";
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            err_message = "Unsupported socket operation error";
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            err_message = "Unfinished socket operation error";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            err_message = "Proxy authentication required error";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            err_message = "SSL handshake failed error";
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            err_message = "Proxy connection refused error";
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            err_message = "Proxy connection closed error";
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            err_message = "Proxy connection timeout error";
            break;
        case QAbstractSocket::ProxyNotFoundError:
            err_message = "Proxy not found error";
            break;
        case QAbstractSocket::ProxyProtocolError:
            err_message = "Proxy protocol error";
            break;
        case QAbstractSocket::OperationError:
            err_message = "Operation error";
            break;
        case QAbstractSocket::SslInternalError:
            err_message = "SSL internal error";
            break;
        case QAbstractSocket::SslInvalidUserDataError:
            err_message = "SSL invalid user data error";
            break;
        case QAbstractSocket::TemporaryError:
            err_message = "Temporary error";
            break;

        default:
            err_message = "Unknown socket error";
    }

    qCritical() << err_message;
}


void ServerConnection::sendMessage(QJsonObject &&data) {

}
