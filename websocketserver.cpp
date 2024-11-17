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

void WebSocketServer::onNewConnection() {
    QWebSocket *clientSocket = m_webSocketServer->nextPendingConnection();

    if (clientSocket) {
        qDebug() << "New client connected:" << clientSocket->peerAddress().toString();
        m_clients.append(clientSocket);

        connect(clientSocket, &QWebSocket::textMessageReceived, this, &WebSocketServer::onTextMessageReceived);  // Menghubungkan sinyal ke slot yang benar
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

void WebSocketServer::onTextMessageReceived(const QString &message) {
    qDebug() << "Received message:" << message;
    processMessage(message);  // Proses pesan yang diterima
}

//void WebSocketServer::processMessage(const QString &message) {
//    qDebug() << "Processing message:" << message;
//    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

//    if (doc.isObject()) {
//        QJsonObject obj = doc.object();
//        QString command = obj["command"].toString();  // Ambil perintah dari pesan

//        if (command == "initDevice") {
//            emit m_mainWindow->initDevice();  // Emit signal untuk init device
//            sendResponse("InitDevice success", 0);
//        }
//        else if (command == "unInitDevice") {
//            emit m_mainWindow->uninitDevice();  // Emit signal untuk uninit device
//            sendResponse("UninitDevice success", 0);
//        }
//       else if (command == "thumbFinger") {
//           emit m_mainWindow->thumbFinger();
//           sendResponse("thumbFinger success", 0);
//       }
//        else {
//            sendResponse("Invalid command", -1);  // Kirim response jika perintah tidak valid
//        }
//    } else {
//        sendResponse("Invalid JSON format", -1);  // Kirim response jika format JSON tidak valid
//    }
//}


void WebSocketServer::processMessage(const QString &message) {
    qDebug() << "Received message:" << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        QString command = obj["command"].toString(); // Ambil perintah dari pesan

        if (command == "initDevice") {
            // Panggil fungsi initDevice dan ambil hasilnya
            QString result = m_mainWindow->initDevice();

            // Analisis hasil untuk menentukan respon
            if (result.contains("success")) {
                sendResponse("InitDevice success", 0);
            } else if (result.contains("fail")) {
                // Ekstrak kode error dari hasil string
                QRegularExpression re("ret: (-?\\d+)");
                QRegularExpressionMatch match = re.match(result);
                int errorCode = match.hasMatch() ? match.captured(1).toInt() : -1;

                sendResponse("InitDevice fail", errorCode);
            }
        } else if (command == "unInitDevice") {
            // Panggil fungsi uninitDevice untuk mendapatkan hasil
            QString result = m_mainWindow->uninitDevice();

            if (result.contains("Device not initialized")) {
                // Jika perangkat belum diinisialisasi, kirim respons dengan kode -2
                sendResponse("Device not initialized", -2);
            } else if (result.contains("success")) {
                // Jika berhasil, kirim respons sukses
                sendResponse("UnInitDevice success", 0);
            } else if (result.contains("fail")) {
                // Jika gagal, ekstrak kode error dari hasil string
                QRegularExpression re("ret: (-?\\d+)");
                QRegularExpressionMatch match = re.match(result);
                int errorCode = match.hasMatch() ? match.captured(1).toInt() : -1;

                // Kirim respons gagal dengan kode error yang sesuai
                sendResponse("UnInitDevice fail", errorCode);
            }
        } else {
            sendResponse("Invalid command", -1); // Kirim response jika perintah tidak valid
        }
    } else {
        sendResponse("Invalid JSON format", -1); // Kirim response jika format JSON tidak valid
    }
}


WebSocketServer::~WebSocketServer() {
    if (m_webSocketServer) {
        m_webSocketServer->close();
    }
    qDeleteAll(m_clients);
    qDebug() << "WebSocket server stopped and all clients disconnected";
}
