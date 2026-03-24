#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QDateTime>
#include "DeviceInfo.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief 載入目前插入電腦的 com port
     */
    void loadPorts();

    /**
     * @brief 顯示序列阜裝置訊息
     * @param
     */
    void showDeviceInfo(int );
    void on_openBtn_clicked();

    /**
     * @brief 關閉序列阜
     */
    void closePort();
    void sendData();

    void on_clearLogBtn_clicked();

private:
    Ui::MainWindow *ui;
    QList<DeviceInfo> deviceList;
    QSerialPort* serialPort;

    void initWidgets();

    /**
     * @brief 開啟序列阜
     * @param portName: COM 名稱。
     * @param baudRate: 鮑率。
     * @return 是否開啟成功。
     */
    bool openPort(const QString& portName, const int& baudRate);
    void appendPacketLog(const QString& direction, const QByteArray& data);
};
#endif // MAINWINDOW_H
