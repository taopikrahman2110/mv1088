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


void kamera::takePhoto(const QString &ip) {
    QUrl url("http://" + ip + ":6002/mvfacial_terminal");

    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

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

    QNetworkReply *reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, [reply, ip, this]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error occurred: " << reply->errorString();
            emit photoTaken(QJsonObject(), -1);  // Emit empty JSON for error
            reply->deleteLater();
            return;
        }

        QByteArray responseBytes = reply->readAll();
        qDebug() << "Response received: " << responseBytes;

        QJsonDocument responseDoc = QJsonDocument::fromJson(responseBytes);
        if (!responseDoc.isObject()) {
            qDebug() << "Invalid JSON response";
            emit photoTaken(QJsonObject(), -1);
            reply->deleteLater();
            return;
        }

        QJsonObject responseObj = responseDoc.object();
        qDebug() << "Full response JSON:" << QJsonDocument(responseObj).toJson(QJsonDocument::Indented);

        QJsonObject params = responseObj.value("params").toObject();
        if (params.isEmpty()) {
            qDebug() << "Params object is empty";
            emit photoTaken(QJsonObject(), -1);
            reply->deleteLater();
            return;
        }

        QString imagePath = params.value("image_path").toString();
        qDebug() << "Extracted image path:" << imagePath;

        if (imagePath.isEmpty()) {
            qDebug() << "Image path is empty";
            emit photoTaken(responseObj, 0);
            reply->deleteLater();
            return;
        }

        QString imageUrl = QString("http://%1:80%2").arg(ip).arg(imagePath);
        qDebug() << "Built image URL:" << imageUrl;

        QUrl validImageUrl(imageUrl);
        if (!validImageUrl.isValid()) {
            qDebug() << "Invalid image URL:" << validImageUrl.toString();
            emit photoTaken(QJsonObject(), -1);
            reply->deleteLater();
            return;
        }

        QNetworkRequest imageRequest(validImageUrl);
        QNetworkReply *imageReply = m_networkManager->get(imageRequest);

        connect(imageReply, &QNetworkReply::finished, [imageReply, responseObj, this]() {
            if (imageReply->error() != QNetworkReply::NoError) {
                qDebug() << "Error fetching image: " << imageReply->errorString();
                emit photoTaken(QJsonObject(), -1);
                imageReply->deleteLater();
                return;
            }

            QByteArray imageData = imageReply->readAll();
            qDebug() << "Image data size:" << imageData.size();

            QString base64Image = QString::fromLatin1(imageData.toBase64());
            qDebug() << "Base64 image string (first 100 chars):" << base64Image.left(100);

            // Tambahkan "image_result" ke dalam "params"
            QJsonObject params = responseObj.value("params").toObject();
            params["image_result"] = base64Image;  // Tambahkan key baru untuk Base64

            // Debug params yang telah diperbarui
            qDebug() << "Updated params JSON:" << QJsonDocument(params).toJson(QJsonDocument::Indented);

            // Masukkan kembali params yang diperbarui ke dalam responseObj
            responseObj["params"] = params;

            // Debug responseObj yang telah diperbarui
            qDebug() << "Updated responseObj JSON:" << QJsonDocument(params).toJson(QJsonDocument::Indented);

            // Debug JSON yang dikirimkan melalui emit
            qDebug() << "JSON sent via emit:" << QJsonDocument(params).toJson(QJsonDocument::Indented);

            emit photoTaken(params, 0);  // Gunakan responseObj yang diperbarui
            imageReply->deleteLater();
        });

        reply->deleteLater();
    });
}









void kamera::setDisplay(const QString &ip, const QString show_contents)
{

    QUrl url("http://" + ip + "/facial_api/aiot_call");


    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        emit setDisplayed(QJsonObject(), -1);  // Emit empty JSON for error
        return;  // Keluar dari fungsi jika URL tidak valid
    }

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

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
    QUrl url("http://" + ip + "/facial_api/aiot_call");

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
    QUrl url("http://" + ip + ":6002/mvfacial_terminal");
    if (!url.isValid()) {
        qDebug() << "Invalid URL:" << url.toString();
        return;  // Keluar jika URL tidak valid
    }

    QNetworkRequest request(url);

    // Set header untuk Content-Type sebagai JSON
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    payload["id"] = 1;
    payload["jsonrpc"] = "2.0";
    payload["method"] = "collect_stop_sync";
    payload["params"] = NULL;

    QJsonDocument doc(payload);
    QByteArray data = doc.toJson();

    // Kirim POST request menggunakan network manager
    m_networkManager->post(request, data);

    // Tidak perlu menangani respons, hanya mengirimkan permintaan
}

void kamera::getPhoto(const QString &path, const QString ip)
{

}












