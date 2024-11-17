#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <MV1088_TYPE.h>
#include <MV1088_API.h>
#include <qdir.h>
#include <QByteArray>
#include <QBuffer>
#include "websocketserver.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

QString imageToBase64(const QImage &image) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "jpg");  // Simpan sebagai PNG atau format lain
    return byteArray.toBase64();
}

MainWindow *mythis;
bool isInitDevice = false;
int regmode = 0;   //指纹采集模式 1 双指采集 2 左手四指 3 右手四指 4 滚指
int fingerNum = -1;


static int CALLBACK MYZW_CallbackPreviewImage(void *pContext, const ZWImageData ZWImgData)
{
    qDebug() << "Panggilan balik gambar diterima";


#if 1
    if(!isInitDevice)
        return -1;

    QJsonArray fingerDataArray;

    emit mythis->signal_settip("Acquisition mode：" + QString::number(regmode));
    emit mythis->signal_settip("FingerCount：" + QString::number(ZWImgData.FingerCount)); // jumlah sidik jari yang terdeteksi
    emit mythis->signal_settip("FingerLR：" + QString::number(ZWImgData.FingerLR)); // posisi jari
    emit mythis->signal_settip("FingerEdge：" + QString::number(ZWImgData.FingerEdge)); // jarak tepi

    bool issucc = true;
    QList<QLabel> lblist;

    // Proses semua detail sidik jari yang terdeteksi
    for (int i = 0; i < ZWImgData.FingerCount; i++) {
        ZWFingerDetail Detail = ZWImgData.FingerDetails[i];

        emit mythis->signal_settip("Finger quality：" + QString::number(Detail.quality));
        emit mythis->signal_settip("X：" + QString::number(Detail.x));
        emit mythis->signal_settip("Y：" + QString::number(Detail.y));
        emit mythis->signal_settip("Image width：" + QString::number(Detail.width));
        emit mythis->signal_settip("Image height：" + QString::number(Detail.height));

        // Cek kualitas sidik jari (quality)
        if (Detail.quality < 30) {
            issucc = false;
        }
    }

    if (regmode == 1) {
        if (ZWImgData.FingerCount == 2) {
            // Validasi jarak tepi
            if (ZWImgData.FingerEdge == -1) {
                emit mythis->signal_settip("Fingers are too close to the left edge");
                return 0;
            } else if (ZWImgData.FingerEdge == -2) {
                emit mythis->signal_settip("Fingers are too close to the right edge");
                return 0;
            } else if (ZWImgData.FingerEdge == -3) {
                emit mythis->signal_settip("Fingers are too close to the upper edge");
                return 0;
            }

            // Jika kualitas baik, lanjutkan dengan pemrosesan
            if (issucc) {
                for (int i = 0; i < ZWImgData.FingerCount; i++) {
                    ZWFingerDetail Detail = ZWImgData.FingerDetails[i];

                    // Membaca dan menyimpan gambar sidik jari
                    QImage image((const unsigned char *)Detail.Buffer, 400, 500, QImage::Format_Grayscale8);
                    QString imagename = QString("./fingerimage/thumb_%1.jpg").arg(QString::number(i));
                    image.save(imagename);

                    QString base64String = imageToBase64(image);
                  //  qDebug() << "hasil base 64" << base64String;

                    // Menyimpan base64 string ke file
                    QString base64FileName = QString("./fingerimage/thumb_%1.txt").arg(QString::number(i));
                    QFile base64File(base64FileName);
                    if (base64File.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&base64File);
                        out << base64String;  // Simpan string Base64 ke file
                        base64File.close();
                    } else {
                        qDebug() << "Failed to open file for writing Base64:" << base64FileName;
                    }

                    // Menambahkan data ke fingerDataArray
                    QJsonObject fingerData;
                    fingerData["fingerFile"] = imagename;
                    fingerData["base64Image"] = base64String;
                    fingerData["quality"] = Detail.quality;  // Menyertakan kualitas sidik jari
                    fingerData["fingerName"] = QString("thumb_%1").arg(QString::number(i));
                    fingerDataArray.append(fingerData);
                }

                // Simpan gambar mentah
                QImage image1((const unsigned char *)ZWImgData.Buffer, 1600, 1500, QImage::Format_Grayscale8);
                image1.save("./fingerimage/raw.jpg");

                emit mythis->signal_settip("Acquisition of two fingers was successful");
                emit mythis->signal_sendFingerData("thumbFinger success", 0, fingerDataArray);
                return 1;
            }
        } else {
            emit mythis->signal_settip("Press both fingers");
        }
    }

    else if (regmode == 2)
    {
        if(ZWImgData.FingerCount == 4)
        {
            if(ZWImgData.FingerLR == 1)
            {
                if(ZWImgData.FingerEdge == -1)
                {
                    emit mythis->signal_settip("Fingers are too close to the right edge");
                    return 0;
                }
                else if(ZWImgData.FingerEdge == -2)
                {
                    emit mythis->signal_settip("Fingers are too close to the right edge");
                    return 0;
                }
                else if(ZWImgData.FingerEdge == -3)
                {
                    emit mythis->signal_settip("Fingers are too close to the upper edge");
                    return 0;
                }
                if(issucc)
                {
                    for(int i = 0;i < ZWImgData.FingerCount;i++)
                    {
                        ZWFingerDetail Detail = ZWImgData.FingerDetails[i];

                        QImage image((const unsigned char *)ZWImgData.FingerDetails[i].Buffer, 400, 500, QImage::Format_Grayscale8);
                        QString imagename = QString("./fingerimage/left_%1.jpg").arg(QString::number(i));
                        image.save(imagename);

                        QString base64String = imageToBase64(image);
                        QString base64FileName = QString("./fingerimage/left_%1.txt").arg(QString::number(i));

                        QFile base64File(base64FileName);
                        if (base64File.open(QIODevice::WriteOnly | QIODevice::Text)) {
                            QTextStream out(&base64File);
                            out << base64String;  // Simpan string Base64 ke file
                            base64File.close();
                        } else {
                            qDebug() << "Failed to open file for writing Base64:" << base64FileName;
                        }
                        QJsonObject fingerData;
                        fingerData["fingerFile"] = imagename;
                        fingerData["base64Image"] = base64String;
                        fingerData["quality"] = Detail.quality;  // Menyertakan kualitas sidik jari
                        fingerData["fingerName"] = QString("left_%1").arg(QString::number(i));
                        fingerDataArray.append(fingerData);


                    }
                    QImage image1((const unsigned char *)ZWImgData.Buffer, 1600, 1500, QImage::Format_Grayscale8);
                    image1.save("./fingerimage/raw.jpg");
                    emit mythis->signal_settip("The four fingers of the left hand were successfully collected");
                    emit mythis->signal_sendFingerData("leftFinger success", 0, fingerDataArray);

                    return 1;
                }
            }
            else
            {
                emit mythis->signal_settip("Please collect the left hand");
            }
        }
        else
        {
            emit mythis->signal_settip("Please collect four fingers");
        }
    }
    else if(regmode == 3)
    {
        if(ZWImgData.FingerCount == 4)
        {
            if(ZWImgData.FingerLR == 2)
            {
                if(ZWImgData.FingerEdge == -1)
                {
                    emit mythis->signal_settip("Fingers are too close to the right edge");
                    return 0;
                }
                else if(ZWImgData.FingerEdge == -2)
                {
                    emit mythis->signal_settip("Fingers are too close to the right edge");
                    return 0;
                }
                else if(ZWImgData.FingerEdge == -3)
                {
                    emit mythis->signal_settip("Fingers are too close to the upper edge");
                    return 0;
                }
                if(issucc)
                {
                    for(int i = 0;i < ZWImgData.FingerCount;i++)
                    {
                        QImage image((const unsigned char *)ZWImgData.FingerDetails[i].Buffer, 400, 500, QImage::Format_Grayscale8);
                        QString imagename = QString("./fingerimage/right_%1.jpg").arg(QString::number(i));
                        image.save(imagename);

                    }
                    QImage image1((const unsigned char *)ZWImgData.Buffer, 1600, 1500, QImage::Format_Grayscale8);
                    image1.save("./fingerimage/raw.jpg");
                    emit mythis->signal_settip("The collection of the four fingers of the right hand was successful");
                    return 1;
                }
            }
            else
            {
                emit mythis->signal_settip("Please collect the right hand");
            }
        }
        else
        {
            emit mythis->signal_settip("Please collect four fingers");
        }
    }
    return 0;
#endif
}

QString MainWindow::initDevice() {
    ZWDeviceInitParam ptInfo = {};
    ptInfo.modelPath = "./runs/subApps/";
    ptInfo.iBright = 10;
    ptInfo.iContrast = 70;
    ptInfo.tCallBackPreview = (MV_CallbackPreviewImage)MYZW_CallbackPreviewImage;

    int ret = MV1088_InitDevice(&ptInfo);
    QString statusMessage = QString("InitDevice ret: %1").arg(ret);

    if (ret == 0) {
        statusMessage.prepend("InitDevice success\n");
        isInitDevice = true;
        HWND hwnd = (HWND)ui->label->winId();
        MV1088_SetVideoWindow(hwnd);
    } else {
        statusMessage.prepend("InitDevice fail\n");
    }

    // Tampilkan di antarmuka pengguna
    emit signal_settip(statusMessage);

    return statusMessage; // Kirim status kembali ke WebSocket
}


QString MainWindow::uninitDevice() {
    // Pastikan perangkat telah diinisialisasi sebelumnya
    if (!isInitDevice) {
        QString statusMessage = "Device not initialized.\n";
        emit signal_settip(statusMessage);  // Kirim pesan ke antarmuka pengguna
        return statusMessage;
    }

    int ret = MV1088_UninitDevice();  // Fungsi untuk un-inisialisasi perangkat

    QString statusMessage = QString("UnInitDevice ret: %1").arg(ret);

    if (ret == 0) {
        statusMessage.prepend("UnInitDevice success\n");
        isInitDevice = false;
    } else {
        statusMessage.prepend("UnInitDevice fail\n");
    }

    // Tampilkan status ke antarmuka pengguna
    emit signal_settip(statusMessage);

    return statusMessage;  // Kirim status kembali ke WebSocket
}

QString MainWindow::thumbFinger() {
    // Periksa apakah perangkat telah diinisialisasi sebelumnya
    if (!isInitDevice) {
        QString statusMessage = "Device not initialized.\n";
        emit signal_settip(statusMessage);  // Kirim pesan ke UI
        return statusMessage;
    }

    // Misalkan Anda memiliki metode untuk menangkap gambar
    int ret = MV1088_StartCapture(2);
    signal_settip("StartCapture ret:"+QString::number(ret));
    QString statusMessage = QString("CaptureThumbs ret: %1").arg(ret);

    if (ret == 0) {
        statusMessage.prepend("CaptureThumbs success\n");
        signal_settip("StartCapture success");
        regmode = 1;

    } else {
        statusMessage.prepend("CaptureThumbs fail\n");
    }

    // Tampilkan status di UI
    emit signal_settip(statusMessage);

    return statusMessage;  // Kirim status kembali ke WebSocket
}

QString MainWindow::leftFourFinger() {
    // Periksa apakah perangkat telah diinisialisasi sebelumnya
    if (!isInitDevice) {
        QString statusMessage = "Device not initialized.\n";
        emit signal_settip(statusMessage);  // Kirim pesan ke UI
        return statusMessage;
    }

    // Misalkan Anda memiliki metode untuk menangkap gambar
    int ret = MV1088_StartCapture(0);
    signal_settip("StartCapture ret:"+QString::number(ret));
    QString statusMessage = QString("leftFourFinger ret: %1").arg(ret);

    if (ret == 0) {
        statusMessage.prepend("leftFourFinger success\n");
        signal_settip("StartCapture success");
        regmode = 2;

    } else {
        statusMessage.prepend("leftFourFinger fail\n");
    }

    // Tampilkan status di UI
    emit signal_settip(statusMessage);

    return statusMessage;  // Kirim status kembali ke WebSocket
}



void MainWindow::sendResponse(const QString &message, int code, const QJsonArray &fingerData) {
    QJsonObject response;
    response["message"] = message;
    response["code"] = code;

    if (!fingerData.isEmpty()) {
        response["fingerData"] = fingerData;
    }

    QJsonDocument doc(response);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    // Kirim melalui WebSocketServer
    if (websocketserver) {
        websocketserver->broadcastMessage(jsonString);
    } else {
        qDebug() << "WebSocket server is not initialized";
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , websocketserver(new WebSocketServer(this, this))

{
    ui->setupUi(this);
    mythis = this; // Masih digunakan untuk callback
    this->setWindowTitle("MV1088Test");
    connect(this, &MainWindow::signal_settip, this, &MainWindow::Addtip);
    connect(this, &MainWindow::signal_sendFingerData, this, [this](const QString &message, int statusCode, const QJsonArray &fingerData) {
        sendResponse(message, statusCode, fingerData);
    });

    createfolder("fingerimage");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Addtip(const QString &tip)
{
    ui->textBrowser->append(tip);
}

void MainWindow::createfolder(const QString &Name)
{
    QDir currentDir = QDir::current();
    if (!currentDir.exists(Name))
    {
        bool success = currentDir.mkpath(Name);
        if (success)
        {
            qDebug() << "文件夹" << Name << "创建成功";
        }
        else
        {
            qDebug() << "无法创建文件夹" << Name;
        }
    }
    else
    {
        qDebug() << "文件夹" << Name << "已经存在";
    }
}



//init device
void MainWindow::on_pushButton_clicked()
{
    ZWDeviceInitParam  ptInfo = {};
    ptInfo.modelPath = "./runs/subApps/";
    ptInfo.iBright = 10;
    ptInfo.iContrast = 70;
    ptInfo.tCallBackPreview = (MV_CallbackPreviewImage)MYZW_CallbackPreviewImage;
    int ret = MV1088_InitDevice(&ptInfo);
    qDebug() << "ret:" << ret;
    signal_settip("InitDevice ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("InitDevice success");
        isInitDevice = true;
        HWND hwnd = (HWND)ui->label->winId();
        MV1088_SetVideoWindow(hwnd);
    }
    else
    {
        signal_settip("InitDevice fail");
    }
}

//uninit device
void MainWindow::on_pushButton_2_clicked()
{
    int ret = MV1088_UninitDevice();
    signal_settip("UninitDevice ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("UninitDevice success");
        isInitDevice = false;
    }
    else
    {
        signal_settip("UninitDevice fail");
    }
}

//empat tangan kiri
void MainWindow::on_pushButton_3_clicked()
{
    if(!isInitDevice)
    {
        signal_settip("Please InitDevice");
        return;
    }
    int ret = MV1088_StartCapture(0);
    signal_settip("StartCapture ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("StartCapture success");
        regmode = 2;

    }
    else
    {
        signal_settip("StartCapture fail");
    }
}

//empat tangan kanan
void MainWindow::on_pushButton_4_clicked()
{
    if(!isInitDevice)
    {
        signal_settip("Please InitDevice");
        return;
    }

    int ret = MV1088_StartCapture(1);
    signal_settip("StartCapture ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("StartCapture success");
        regmode = 3;

    }
    else
    {
        signal_settip("StartCapture fail");
    }
}

//2 jempol
void MainWindow::on_pushButton_5_clicked()
{
    if(!isInitDevice)
    {
        signal_settip("Please InitDevice");
        return;
    }

    int ret = MV1088_StartCapture(2);
    signal_settip("StartCapture ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("StartCapture success");
        regmode = 1;

    }
    else
    {
        signal_settip("StartCapture fail");
    }
}

//stop capture success
void MainWindow::on_pushButton_6_clicked()
{
    if(!isInitDevice)
    {
        signal_settip("Please InitDevice");
        return;
    }

    int ret = MV1088_StopCapture();
    signal_settip("StopCapture ret:"+QString::number(ret));
    if(ret == 0)
    {
        signal_settip("StopCapture success");
        regmode = -1;

    }
    else
    {
        signal_settip("StopCapture fail");
    }
}
