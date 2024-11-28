#include "websocketserver.h"
#include "mainwindow.h"
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QDebug>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

WebSocketServer::WebSocketServer(MainWindow *mainWindow, QObject *parent)
    : QObject(parent),
      m_webSocketServer(new QWebSocketServer(QStringLiteral("My Server"),
                                             QWebSocketServer::SecureMode, this)),
      m_mainWindow(mainWindow),
      m_kamera(new kamera(this))
{
    qDebug() << "SSL Supported:" << QSslSocket::supportsSsl();
    qDebug() << "SSL Library Version:" << QSslSocket::sslLibraryVersionString();
    qDebug() << "SSL Build Version:" << QSslSocket::sslLibraryBuildVersionString();

    QString baseDir = QCoreApplication::applicationDirPath();


    QString certificatePath = QDir::toNativeSeparators(baseDir + "/certs/device.gakkum.local+1.pem");
    QString privateKeyPath = QDir::toNativeSeparators(baseDir + "/certs/device.gakkum.local+1-key.pem");

    qDebug() << "Certificate Path:" << certificatePath;
    qDebug() << "Private Key Path:" << privateKeyPath;

    if (!QFile::exists(certificatePath)) {
        qWarning() << "Certificate file does not exist at:" << certificatePath;
        return;
    }
    if (!QFile::exists(privateKeyPath)) {
        qWarning() << "Private key file does not exist at:" << privateKeyPath;
        return;
    }

    // Muat sertifikat
    QFile certFile(certificatePath);
    if (!certFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open certificate file at:" << certificatePath;
        return;
    }
    QSslCertificate certificate(certFile.readAll(), QSsl::Pem);
    certFile.close();

    if (certificate.isNull()) {
        qWarning() << "Failed to load certificate from:" << certificatePath;
        return;
    }

    QFile keyFile(privateKeyPath);
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open private key file at:" << privateKeyPath;
        return;
    }
    QSslKey privateKey(keyFile.readAll(), QSsl::Rsa);
    keyFile.close();

    if (privateKey.isNull()) {
        qWarning() << "Failed to load private key from:" << privateKeyPath;
        return;
    }

    QSslConfiguration sslConfiguration;
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(privateKey);
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer); // Abaikan validasi untuk testing
    sslConfiguration.setProtocol(QSsl::TlsV1_2);

    m_webSocketServer->setSslConfiguration(sslConfiguration);

    if (m_webSocketServer->listen(QHostAddress::Any, 8787)) {
        qDebug() << "WebSocket Secure Server started on wss://localhost:8787";
        connect(m_webSocketServer, &QWebSocketServer::newConnection, this, &WebSocketServer::onNewConnection);
    } else {
        qWarning() << "Failed to start WebSocket Secure Server on port 8080";
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
        } else if (command == "cekKoneksi"){
            QString ip = obj["ip"].toString();
            kamera cam;
            QString result = cam.cekKoneksi(ip);
            sendResponse(result, 0);
        } else if (command == "takePhoto") {
            if (m_kamera) {
                QString ip = obj["ip"].toString();
                // Menyambungkan sinyal photoTaken ke slot sendResponseKamera untuk menangani hasil foto
                disconnect(m_kamera, &kamera::photoTaken, this, &WebSocketServer::sendResponseKamera);
                connect(m_kamera, &kamera::photoTaken, this, &WebSocketServer::sendResponseKamera);


                m_kamera->takePhoto(ip);
                m_kamera->cancelPhoto(ip);
            } else {
                sendResponseKamera(QJsonObject(), -1);  // Jika kamera tidak tersedia
            }
        }else if (command == "getPhoto")
        {
            if (m_kamera) {
                // Ambil IP dan path dari JSON objek
                QString ip = obj["ip"].toString();
                QString path = obj["path"].toString();

                // Validasi data IP dan path
                if (ip.isEmpty() || path.isEmpty()) {
                    qDebug() << "Invalid IP or path received from user";
                    sendResponseKamera(QJsonObject(), -1); // Kirim error jika data tidak valid
                    return;
                }

                // Pastikan koneksi sinyal tidak terduplikasi (seharusnya hanya sekali di dalam konstruktor atau setup)
                if (!m_photoFetchedConnected) {
                    connect(m_kamera, &kamera::photoFetched, this, [this](const QByteArray &data, int status) {
                        if (status == 0) {
                            // Kirim data biner jika berhasil
                            if (m_webSocket) {
                                qDebug() << "Sending binary photo data to the client";
                                m_webSocket->sendBinaryMessage(data); // Kirim data biner
                            }
                        } else {
                            // Kirim pesan error dalam bentuk teks jika gagal
                            QJsonObject response;
                            response["error"] = "Failed to fetch photo.";
                            sendResponseKamera(response, status);
                        }
                    });
                    m_photoFetchedConnected = true;
                }

                // Panggil fungsi getPhoto pada kamera
                m_kamera->getPhoto(path, ip);

            } else {
                qDebug() << "Camera object not available";
                sendResponseKamera(QJsonObject(), -1); // Kirim error jika kamera tidak tersedia
            }
        } else if (command == "setDisplay"){
            if (m_kamera) {
                QString ip = obj["ip"].toString();
                QString show_contents = obj["show_contents"].toString();
                // Menyambungkan sinyal photoTaken ke slot sendResponseKamera untuk menangani hasil foto
                disconnect(m_kamera, &kamera::setDisplayed, this, &WebSocketServer::sendResponseKamera);
                connect(m_kamera, &kamera::setDisplayed, this, &WebSocketServer::sendResponseKamera);


                m_kamera->setDisplay(ip, show_contents);
            } else {
                sendResponseKamera(QJsonObject(), -1);  // Jika kamera tidak tersedia
            }
        } else if (command == "getDisplay"){
            if (m_kamera) {
                QString ip = obj["ip"].toString();
                // Menyambungkan sinyal photoTaken ke slot sendResponseKamera untuk menangani hasil foto
                disconnect(m_kamera, &kamera::getDisplayed, this, &WebSocketServer::sendResponseKamera);
                connect(m_kamera, &kamera::getDisplayed, this, &WebSocketServer::sendResponseKamera);


                m_kamera->getDisplay(ip);
            } else {
                sendResponseKamera(QJsonObject(), -1);  // Jika kamera tidak tersedia
            }
        }
        else {
            sendResponse("Invalid command", -1);
        }
    } else {
        sendResponse("Invalid JSON format", -1);
    }
}


void WebSocketServer::sendResponseKamera(QJsonObject jsonObj, int code) {
    // Membuat response object dengan message berupa jsonObj
    QJsonObject response;
    response["message"] = jsonObj;  // Menyisipkan objek JSON sebagai bagian dari response
    response["ret"] = code;

    // Mengonversi objek response menjadi string JSON dalam format Compact
    QJsonDocument doc(response);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    // Mengirimkan response JSON ke semua klien WebSocket yang terhubung
    for (QWebSocket *clientSocket : qAsConst(m_clients)) {
        if (clientSocket->isValid()) {
            clientSocket->sendTextMessage(jsonString);  // Kirim JSON string ke WebSocket
        }
    }

    // Debug log untuk memastikan JSON telah dikirim
    qDebug() << "Response sent:" << jsonString;
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
