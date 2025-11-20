//
// Created by fsi on 16.11.2025.
//

#include "server/ServerConnection.h"
#include "utils/enums.h"
#include "utils/enumWrapper.h"
#include <qjsonobject.h>
#include <qjsondocument.h>

ServerConnection::ServerConnection() {
    setConnections();
    m_ws.open(QUrl(QStringLiteral("ws://127.0.0.1:8080/api/users")));
}


ServerConnection::~ServerConnection() {
    m_ws.close(QWebSocketProtocol::CloseCodeNormal, "finish");
}


void ServerConnection::setConnections() {
    connect(&m_ws, &QWebSocket::connected, this, &ServerConnection::onConnected);
    connect(&m_ws, &QWebSocket::disconnected, this, &ServerConnection::onDisconnected);
    connect(&m_ws, &QWebSocket::textMessageReceived, this, &ServerConnection::onReceivedMessage);
    connect(&m_ws, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ServerConnection::onError);
}


void ServerConnection::requestForAddUser(const QJsonObject &data) {
    QJsonObject request;
    request["action"] = QJsonValue("insert");
    request["data"] = data;

    sendMessage(std::move(request));
}


void ServerConnection::requestForDeleteUser(const QJsonObject &data) {
    QJsonObject request;
    request["action"] = QJsonValue("delete");
    request["data"] = data;

    sendMessage(std::move(request));
}


void ServerConnection::requestForUsersList() {
    QJsonObject request;
    request["action"] = QJsonValue("select");

    sendMessage(std::move(request));
}


void ServerConnection::onConnected() {
    qDebug() << "Connected";
}


void ServerConnection::onDisconnected() {
    qCritical() << "Disconnected";
}


void ServerConnection::onReceivedMessage(const QString &message) {
    qDebug() << "onReceiveMessage";

    if(!(message.startsWith('{') || message.startsWith("["))){
        qCritical() << "Message stucture is invalid: " << message << "!\nJSON expected!";
        return;
    }

    QJsonDocument jsonDoc;
    QJsonParseError err(QJsonParseError::NoError);
    jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &err);

    if(err.error != QJsonParseError::NoError) {
        qCritical() << "Ошибка парсинга JSON: " << err.errorString();
        return;
    }

    if(!jsonDoc.isObject()) {
        qCritical() << "JSON error: json object expected!";
        return;
    }

    QJsonObject obj = jsonDoc.object();
    handleJSON(std::move(obj));
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
    QJsonDocument jsonDoc(data);
    m_ws.sendTextMessage(jsonDoc.toJson(QJsonDocument::Compact));
}


void ServerConnection::handleJSON(QJsonObject &&obj) {
    QJsonValue jsonStatus = obj["jsonStatus"],
            jsonAction = obj["jsonAction"];

    if(jsonStatus.isUndefined()){
        qCritical() << "There is no jsonStatus!";
        return;
    }

    if(jsonAction.isUndefined()) {
        qCritical() << "There is unknown jsonAction!";
        return;
    }

    std::string statusStr = jsonStatus.toString().toStdString(),
            actionStr = jsonAction.toString().toStdString();

    Status status = enums::wrap::fromString<Status>(std::move(statusStr));
    Action action = enums::wrap::fromString<Action>(std::move(statusStr));

    if(status == Status::UNKNOWN)
        qCritical() << "Status of executing request is unknown";
    else if(status == Status::ERROR)
        qCritical() << "Error of executing request: " << obj["data"].toString();
    else {
        switch (action) {
            case Action::SELECT:
                emit selectResponse(obj["data"].toArray());
                break;
            case Action::INSERT:
                emit insertResponse(obj["data"].toObject());
            case Action::DELETE:
                emit deleteResponse(obj["data"].toObject());
            default:
                qCritical() << "Unknown action with this status!";
        }
    }
}
