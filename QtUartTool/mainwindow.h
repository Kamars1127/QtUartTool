#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
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
     * @brief 顯示序列埠裝置訊息
     * @param
     */
    void showDeviceInfo(int );
    void on_openBtn_clicked();

    /**
     * @brief 關閉序列埠
     */
    void closePort();
    void sendData();
    void readData();

    void on_clearLogBtn_clicked();

    void handleSerialPortError(QSerialPort::SerialPortError error); //處理序列埠錯誤

private:
    Ui::MainWindow *ui;
    QList<DeviceInfo> deviceList; //序列埠裝置資訊列表
    QSerialPort* serialPort;
    QTimer* rxTimer;
    QByteArray rxBuffer; //Rx 資料緩衝區

    void initUi();

    /**
     * @brief 設定序列埠開啟或關閉時的 UI 狀態
     * @param 序列埠是否已開啟。
     */
    void setPortOpenUi(const bool isOpen);
    /**
     * @brief 開啟序列埠
     * @param portName: COM 名稱。
     * @param baudRate: 鮑率。
     * @return 是否開啟成功。
     */
    bool openPort(const QString& portName, const int& baudRate);

    /**
     * @brief 新增一筆 Log 到 table 中。
     * @param direction: 資料流方向。
     * @param data: 發送或接收的資料。
     */
    void appendPacketLog(const QString& direction, const QByteArray& data);

    /**
     * @brief 把 Rx 資料緩衝區裡目前累積的資料，一次吐出來處理。
     */
    void flushRxBuffer();
};
#endif // MAINWINDOW_H
