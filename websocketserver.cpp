#include "websocketserver.h"
#include "mainwindow.h"
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QDebug>

WebSocketServer::WebSocketServer(MainWindow *mainWindow, QObject *parent)
    : QObject(parent),
      m_webSocketServer(new QWebSocketServer(QStringLiteral("My Server"),
                                             QWebSocketServer::NonSecureMode, this)),
      m_mainWindow(mainWindow)
{
    if (m_webSocketServer->listen(QHostAddress::Any, 8080)) {
        qDebug() << "WebSocket server started on port 8080";
        connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
    } else {
        qWarning() << "Failed to start WebSocket server on port 8080";
    }
}

WebSocketServer::~WebSocketServer() {
    if (m_webSocketServer) {
        m_webSocketServer->close();
    }

    QString result = m_mainWindow->uninitDevice();

    qDeleteAll(m_clients); // Hapus semua klien yang masih tersisa
    qDebug() << "WebSocket server stopped and all clients disconnected";
}

void WebSocketServer::onNewConnection() {
    QWebSocket *clientSocket = m_webSocketServer->nextPendingConnection();

    if (clientSocket) {
        qDebug() << "New client connected:" << clientSocket->peerAddress().toString();
        m_clients.append(clientSocket);

        connect(clientSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);
        connect(clientSocket, &QWebSocket::disconnected, this, &WebSocketServer::onDisconnected);
    } else {
        qWarning() << "Failed to retrieve the new client connection.";
    }
}

void WebSocketServer::onDisconnected() {
    QWebSocket *clientSocket = qobject_cast<QWebSocket *>(sender());
    if (clientSocket) {
        qDebug() << "Client disconnected:" << clientSocket->peerAddress().toString();
        m_clients.removeAll(clientSocket);
        clientSocket->deleteLater();
    } else {
        qWarning() << "onDisconnected called, but sender is not a QWebSocket.";
    }
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    qDebug() << "Received message:" << message;
    processMessage(message); // Proses pesan yang diterima
}

void WebSocketServer::processMessage(const QString &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString command = obj["command"].toString(); // Ambil perintah dari pesan

        if (command == "initDevice") {
            QString result = m_mainWindow->initDevice();
            if (result.contains("success")) {
                sendResponse("InitDevice success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("InitDevice fail", errorCode);
            }
        } else if (command == "unInitDevice") {
            QString result = m_mainWindow->uninitDevice();
            if (result.contains("Device not initialized")) {
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                sendResponse("UnInitDevice success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("UnInitDevice fail", errorCode);
            }
        } else if (command == "thumbFinger") {
            QString result = m_mainWindow->thumbFinger();
            if (result.contains("Device not initialized")) {
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                sendResponse("thumbFinger success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("thumbFinger fail", errorCode);
            }
        } else if (command == "leftFourFinger") {
            QString result = m_mainWindow->leftFourFinger();
            if (result.contains("Device not initialized")) {
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                sendResponse("leftFourFinger success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("leftFourFinger fail", errorCode);
            }
        } else if (command == "rightFourFinger") {
            QString result = m_mainWindow->rightFourFinger();
            if (result.contains("Device not initialized")) {
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                sendResponse("rightFourFinger success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("rightFourFinger fail", errorCode);
            }
        } else if (command == "stopCapture") {
            QString result = m_mainWindow->stopCapture();
            if (result.contains("Device not initialized")) {
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                sendResponse("stopCapture success", 0);
            } else {
                int errorCode = extractErrorCode(result);
                sendResponse("stopCapture fail", errorCode);
            }
        } else {
            sendResponse("Invalid command", -1);
        }
    } else {
        sendResponse("Invalid JSON format", -1);
    }
}

void WebSocketServer::sendResponse(const QString &message, int code) {
    QJsonObject responseObj;
    responseObj["message"] = message;
    responseObj["ret"] = code;

    QJsonDocument doc(responseObj);
    QString response = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (QWebSocket *clientSocket : qAsConst(m_clients)) {
        if (clientSocket->isValid()) {
            clientSocket->sendTextMessage(response);
        }
    }
    qDebug() << "Response sent:" << response;
}

void WebSocketServer::broadcastMessage(const QString &message) {
    for (QWebSocket *clientSocket : qAsConst(m_clients)) {
        if (clientSocket->isValid()) {
            clientSocket->sendTextMessage(message);
        }
    }
}

int WebSocketServer::extractErrorCode(const QString &response) {
    QRegularExpression re("ret: (-?\\d+)");
    QRegularExpressionMatch match = re.match(response);
    return match.hasMatch() ? match.captured(1).toInt() : -1;
}
