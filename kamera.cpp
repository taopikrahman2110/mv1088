#include "kamera.h"
#include <QProcess>
#include <QDebug>

kamera::kamera() {
    // Konstruktor kamera
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

    // Jika output mengandung 'Reply' berarti ping berhasil
    if (result.contains("Reply") || result.contains("bytes from")) {
        return "Ping success";
    } else {
        return "Ping failed";
    }
}
