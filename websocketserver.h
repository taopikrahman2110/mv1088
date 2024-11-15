#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>

class MainWindow; // Forward declaration

class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketServer(MainWindow *mainWindow, QObject *parent = nullptr); // Terima pointer ke MainWindow
    ~WebSocketServer();

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void sendResponse(const QString &message, int code);
    void processMessage(const QString &message);



private:

    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket *> m_clients;
    MainWindow *m_mainWindow; // Pointer ke MainWindow
};

#endif // WEBSOCKETSERVER_H
