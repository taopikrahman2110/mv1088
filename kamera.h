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
    void takePhoto(const QString &ip);
    void cancelPhoto(const QString &ip);
    void getPhoto(const QString &path, const QString ip);
    void setDisplay(const QString &ip, const QString show_contents);

private:
    QNetworkAccessManager *m_networkManager; // Pointer untuk Network Manager
    bool isTakingPhoto;


signals:
    void photoTaken(const QJsonObject &json, int code);
    void setDisplayed(const QJsonObject &json, int code);
    void photoFetched(const QByteArray &imageData, int status); // Status 0 untuk sukses, -1 untuk gagal


};

#endif // KAMERA_H
