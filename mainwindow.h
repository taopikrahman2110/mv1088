#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <MV1088_API.h>
#include "websocketserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Addtip(const QString &tip);
    void createfolder(const QString& Name);
    QString initDevice();
    QString uninitDevice();
    QString thumbFinger();




signals:
    void signal_settip(const QString &tip);

public slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    WebSocketServer *websocketserver;

};
#endif // MAINWINDOW_H
