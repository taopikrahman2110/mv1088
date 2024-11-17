#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonArray>
#include <QList>
#include "kamera.h"

class MainWindow;

class WebSocketServer : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketServer(MainWindow *mainWindow, QObject *parent = nullptr);
    ~WebSocketServer();

    void sendResponse(const QString &message, int code);
    void sendResponseKamera(QJsonObject jsonObj, int code);
    void broadcastMessage(const QString &message);

private slots:
    void onNewConnection();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);


private:
    void processMessage(const QString &message);
    int extractErrorCode(const QString &response);

    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket *> m_clients; // Menyimpan klien yang terhubung
    MainWindow *m_mainWindow;      // Pointer ke MainWindow untuk interaksi
    kamera *m_kamera;
};

#endif // WEBSOCKETSERVER_H
