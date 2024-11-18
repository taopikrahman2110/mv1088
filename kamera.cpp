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

void kamera::takePhoto(const QString &ip)
{

    QUrl url("http://" + ip + ":6002/mvfacial_terminal");


    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
        return;  // Keluar dari fungsi jika URL tidak valid
    }

    QNetworkRequest request(url);

    // Set header for Content-Type as JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Create JSON payload with parameters
    QJsonObject params;
    params["show_ui"] = 1;
    params["time_out"] = 30;

    QJsonObject payload;
    payload["id"] = 1;
    payload["jsonrpc"] = "2.0";
    payload["method"] = "collect_start_sync";
    payload["params"] = params;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Kirim POST request menggunakan network manager
    QNetworkReply *reply = m_networkManager->post(request, data);

    // Handle response ketika selesai
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        // Cek error pada reply
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error occurred: " << reply->errorString();  // Debug error
            emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar dari lambda jika terjadi error
        }

        // Baca data dari reply
        QByteArray responseBytes = reply->readAll();
        qDebug() << "Response received: " << responseBytes;  // Debug response

        // Parse response ke QJsonObject
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseBytes);
        if (!responseDoc.isObject()) {
            qDebug() << "Invalid JSON response";  // Invalid JSON structure
            emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar jika response bukan JSON yang valid
        }

        // Parse response yang valid
        QJsonObject responseObj = responseDoc.object();
        emit photoTaken(responseObj, 0);  // Emit photoTaken signal dengan response
        reply->deleteLater();
        return;
    });
}

void kamera::setDisplay(const QString &ip, const QString show_contents)
{

    QUrl url("http://" + ip + ":6002/mvfacial_terminal");


    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit setDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
        return;  // Keluar dari fungsi jika URL tidak valid
    }

    QNetworkRequest request(url);

    // Set header for Content-Type as JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Create JSON payload with parameters
    QJsonObject params;
    params["show_contents"] = show_contents;

    QJsonObject payload;
    payload["method"] = "set_ui_display_tips";
    payload["params"] = params;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Kirim POST request menggunakan network manager
    QNetworkReply *reply = m_networkManager->post(request, data);

    // Handle response ketika selesai
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        // Cek error pada reply
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error occurred: " << reply->errorString();  // Debug error
            emit setDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar dari lambda jika terjadi error
        }

        // Baca data dari reply
        QByteArray responseBytes = reply->readAll();
        qDebug() << "Response received: " << responseBytes;  // Debug response

        // Parse response ke QJsonObject
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseBytes);
        if (!responseDoc.isObject()) {
            qDebug() << "Invalid JSON response";  // Invalid JSON structure
            emit setDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar jika response bukan JSON yang valid
        }

        // Parse response yang valid
        QJsonObject responseObj = responseDoc.object();
        emit setDisplayed(responseObj, 0);  // Emit photoTaken signal dengan response
        reply->deleteLater();
        return;
    });
}

void kamera::getDisplay(const QString &ip)
{
    QUrl url("http://" + ip + ":6002/mvfacial_terminal");

    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit getDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
        return;  // Keluar dari fungsi jika URL tidak valid
    }

    QNetworkRequest request(url);

    // Set header for Content-Type as JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Create JSON payload with parameters
    QJsonObject params;

    QJsonObject payload;
    payload["method"] = "get_ui_display_tips";
    payload["params"] = NULL;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Kirim POST request menggunakan network manager
    QNetworkReply *reply = m_networkManager->post(request, data);

    // Handle response ketika selesai
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        // Cek error pada reply
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error occurred: " << reply->errorString();  // Debug error
            emit getDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar dari lambda jika terjadi error
        }

        // Baca data dari reply
        QByteArray responseBytes = reply->readAll();
        qDebug() << "Response received: " << responseBytes;  // Debug response

        // Parse response ke QJsonObject
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseBytes);
        if (!responseDoc.isObject()) {
            qDebug() << "Invalid JSON response";  // Invalid JSON structure
            emit getDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;  // Keluar jika response bukan JSON yang valid
        }

        // Parse response yang valid
        QJsonObject responseObj = responseDoc.object();
        emit getDisplayed(responseObj, 0);  // Emit photoTaken signal dengan response
        reply->deleteLater();
        return;
    });
}

void kamera::cancelPhoto(const QString &ip)
{
    // Bangun URL dinamis berdasarkan IP yang diterima
    QUrl url("http://" + ip + ":8080/api/testing/facial");
    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        return;  // Keluar jika URL tidak valid
    }

    QNetworkRequest request(url);

    // Set header untuk Content-Type sebagai JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Membuat payload JSON untuk parameter
    QJsonObject params;
    params["show_ui"] = 1;
    params["time_out"] = 30;

    QJsonObject payload;
    payload["id"] = 1;
    payload["jsonrpc"] = "2.0";
    payload["method"] = "collect_stop_sync";
    payload["params"] = params;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Kirim POST request menggunakan network manager
    m_networkManager->post(request, data);

    // Tidak perlu menangani respons, hanya mengirimkan permintaan
}

void kamera::getPhoto(const QString &path, const QString ip)
{
    // URL endpoint dengan path yang disertakan
    QUrl url("http://" + ip + ":8080/api/testing/image/" + path );
    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit photoFetched(QByteArray(), -1); // Emit error jika URL tidak valid
        return;
    }

    QNetworkRequest request(url);

    // Set header untuk Content-Type sebagai JSON (sesuai API server)
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Kirim GET request
    QNetworkReply *reply = m_networkManager->get(request);

    // Handle respons
    connect(reply, &QNetworkReply::finished, [reply, this]() {
        // Cek error
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error fetching photo:" << reply->errorString();
            emit photoFetched(QByteArray(), -1); // Emit error jika gagal
            reply->deleteLater();
            return;
        }

        // Ambil data biner dari respons
        QByteArray imageData = reply->readAll();
        qDebug() << "Photo fetched successfully, size:" << imageData.size();

        // Emit data gambar yang berhasil diambil
        emit photoFetched(imageData, 0); // Emit gambar ke signal
        reply->deleteLater();
    });
}












