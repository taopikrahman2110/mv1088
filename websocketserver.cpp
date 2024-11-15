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

void WebSocketServer::processMessage(const QString &message) {
    qDebug() << "Received message:" << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString command = obj["command"].toString();  // Ambil perintah dari pesan

        if (command == "initdevice") {
            // Memanggil fungsi initdevice di MainWindow
            if (m_mainWindow) {
                m_mainWindow->on_pushButton_clicked();  // Memanggil tombol InitDevice dari MainWindow
            }
            sendResponse("InitDevice success", 0);
        }
        else if (command == "uninitdevice") {
            // Memanggil fungsi uninitdevice di MainWindow
            if (m_mainWindow) {
                m_mainWindow->on_pushButton_2_clicked();  // Memanggil tombol UninitDevice dari MainWindow
            }
            sendResponse("UninitDevice success", 0);
        }
        else {
            sendResponse("Invalid command", -1);  // Kirim response jika perintah tidak valid
        }
    } else {
        sendResponse("Invalid JSON format", -1);  // Kirim response jika format JSON tidak valid
    }
}

void WebSocketServer::sendResponse(const QString &message, int code) {
    QJsonObject responseObj;
    responseObj["message"] = message;
    responseObj["ret"] = code;  // Mengirimkan kode error, 0 untuk sukses, -1 untuk gagal

    QJsonDocument doc(responseObj);
    QString response = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (QWebSocket *clientSocket : qAsConst(m_clients)) {
        clientSocket->sendTextMessage(response);  // Mengirim pesan response ke semua klien
    }

    qDebug() << "Response sent:" << response;
}
