#ifndef KAMERA_H
#define KAMERA_H

#include <QObject>
#include <QNetworkAccessManager>

class kamera : public QObject
{
    Q_OBJECT

public:
    explicit kamera(QObject *parent = nullptr); // Deklarasi konstruktor
    QString cekKoneksi(const QString &ip);
    void takePhoto();

private:
    QNetworkAccessManager *m_networkManager; // Pointer untuk Network Manager


signals:
    void photoTaken(const QJsonObject &json, int code);


};

#endif // KAMERA_H
