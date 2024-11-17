#include "kamera.h"
#include <QProcess>
#include <QDebug>
#include <QtNetwork>
#include <QObject>
#include <QJsonObject>

kamera::kamera(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)) // Inisialisasi di konstruktor
{
    // Inisialisasi tambahan jika diperlukan
}

QString kamera::cekKoneksi(const QString &ip) {
    // Menggunakan QProcess untuk melakukan ping ke IP yang diberikan
    QProcess pingProcess;
    QString command = "ping";
    QStringList arguments;

#ifdef Q_OS_WIN
    arguments << "-n" << "4" << ip;
#else
    arguments << "-c" << "4" << ip;  // Linux/Mac menggunakan -c untuk jumlah ping
#endif

    pingProcess.start(command, arguments);

    if (!pingProcess.waitForStarted()) {
        return "Ping failed to start";
    }

    pingProcess.waitForFinished();
    QByteArray output = pingProcess.readAllStandardOutput();
    QString result = QString::fromUtf8(output);

    // Jika output mengandung 'Reply' atau 'bytes from', berarti ping berhasil
    if (result.contains("Reply") || result.contains("bytes from")) {
        return "Ping success";
    } else {
        return "Ping failed";
    }
}

void kamera::takePhoto()
{
    // URL API
    QUrl url("http://10.18.14.53:8080/api/testing/facial");
    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
        return;
    }

    QNetworkRequest request(url);

    // Set header for Content-Type as JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Create JSON payload with parameters
    QJsonObject params;
    params["show_ui"] = 1;
    params["time_out"] = 20;

    QJsonObject payload;
    payload["id"] = 1;
    payload["jsonrpc"] = "2.0";
    payload["method"] = "collect_start_sync";
    payload["params"] = params;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Send the POST request using the network manager
    QNetworkReply *reply = m_networkManager->post(request, data);

    // Handle the response when finished
    connect(reply, &QNetworkReply::finished, [reply, this]() {
           if (reply->error() == QNetworkReply::NoError) {
               QByteArray responseBytes = reply->readAll();
               qDebug() << "Response received: " << responseBytes;  // Debug response

               // Parse the response to QJsonObject
               QJsonDocument responseDoc = QJsonDocument::fromJson(responseBytes);
               if (!responseDoc.isObject()) {
                   qDebug() << "Invalid JSON response";  // Invalid JSON structure
                   emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
                   reply->deleteLater();
                   return;
               }

               // Successfully parsed the response to QJsonObject
               QJsonObject responseObj = responseDoc.object();
               emit photoTaken(responseObj, 0);  // Emit the photoTaken signal with the response
           } else {
               qDebug() << "Error occurred: " << reply->errorString();  // Debug error
               emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
           }

           reply->deleteLater();
       });
}







