#include "websocketserver.h"
#include "mainwindow.h" // Include MainWindow untuk akses fungsi

WebSocketServer::WebSocketServer(MainWindow *mainWindow, QObject *parent)
    : QObject(parent),
      m_webSocketServer(new QWebSocketServer(QStringLiteral("My Server"),
                                             QWebSocketServer::NonSecureMode, this)),
      m_mainWindow(mainWindow) // Inisialisasi pointer MainWindow
{
    if (m_webSocketServer->listen(QHostAddress::Any, 8080)) {
        qDebug() << "WebSocket server started on port 8080";
        connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
    } else {
        qWarning() << "Failed to start WebSocket server on port 8080";
    }
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


WebSocketServer::~WebSocketServer() {
    if (m_webSocketServer) {
        m_webSocketServer->close();
    }
    qDeleteAll(m_clients);
    qDebug() << "WebSocket server stopped and all clients disconnected";
}

void WebSocketServer::onTextMessageReceived(const QString &message) {
    QWebSocket *clientSocket = qobject_cast<QWebSocket *>(sender());
    if (clientSocket) {
        qDebug() << "Message from client:" << message;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
        if (!jsonDoc.isObject()) {
            clientSocket->sendTextMessage("Invalid message format");
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        QString command = jsonObj.value("command").toString();

        if (command == "initdevice") {
            // Panggil fungsi initDevice di MainWindow
            QString initResult = m_mainWindow->initDevice();
            clientSocket->sendTextMessage(initResult);
        } else {
            clientSocket->sendTextMessage("Unknown command");
        }
    } else {
        qWarning() << "onTextMessageReceived called, but sender is not a QWebSocket";
    }
}
