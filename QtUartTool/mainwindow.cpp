#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPort(new QSerialPort(this))
    , rxTimer(new QTimer(this))

{
    ui->setupUi(this);

    initUi();

    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &MainWindow::loadPorts);

    connect(ui->portNameCmb, &QComboBox::currentIndexChanged,
            this, &MainWindow::showDeviceInfo);

    connect(ui->closeBtn, &QPushButton::clicked,
            this, &MainWindow::closePort);

    connect(ui->sendBtn, &QPushButton::clicked,
            this, &MainWindow::sendData);

    connect(serialPort, &QSerialPort::readyRead,
            this, &MainWindow::readData);

    connect(rxTimer, &QTimer::timeout,
            this, &MainWindow::flushRxBuffer);

    connect(serialPort, &QSerialPort::errorOccurred,
            this, &MainWindow::handleSerialPortError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi(){

    /* 建立鮑率選項 */
    ui->baudRateCmb->addItems({
        "1200","2400","4800","9600","19200","38400","57600","115200","230400","460800","921600"
    });
    ui->baudRateCmb->setCurrentIndex(3); // 預設顯示 9600

    ui->closeBtn->setEnabled(false); // disable Close 按鈕
    ui->sendBtn->setEnabled(false); // disable Send 按鈕

    /* 設定 table 欄位名稱的寬度 */
    ui->packetLogTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Custom);
    ui->packetLogTbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Custom);
    ui->packetLogTbl->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    rxTimer->setSingleShot(true); //將計時器設定成「單次觸發」
}

void  MainWindow::setPortOpenUi(const bool isOpen){
    ui->closeBtn->setEnabled(isOpen);
    ui->sendBtn->setEnabled(isOpen);
    ui->openBtn->setEnabled(!isOpen);
    ui->portNameCmb->setEnabled(!isOpen);
    ui->baudRateCmb->setEnabled(!isOpen);
    ui->refreshBtn->setEnabled(!isOpen);
}

void MainWindow::loadPorts(){
    ui->portNameCmb->clear();
    deviceList.clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {

        DeviceInfo device;
        device.portName = info.portName();
        device.serialNumber = info.serialNumber();
        device.description = info.description();
        device.manufacturer = info.manufacturer();

        deviceList.append(device);
        ui->portNameCmb->addItem(device.portName);
    }
}

void MainWindow::showDeviceInfo(int index){
    //qDebug() << "Item Changed !!";
    //qDebug() << index;

    if(index == -1){
        ui->devicePortNameValueLbl->setText("N/A");
        ui->deviceSerialNumberValueLbl->setText("N/A");
        ui->deviceDescriptionValueLbl->setText("N/A");
        ui->deviceManufacturerValueLbl->setText("N/A");

        return;
    }

    const DeviceInfo& device = deviceList[index];

    ui->devicePortNameValueLbl->setText(device.portName);
    ui->deviceSerialNumberValueLbl->setText(device.serialNumber);
    ui->deviceDescriptionValueLbl->setText(device.description);
    ui->deviceManufacturerValueLbl->setText(device.manufacturer);
}

void MainWindow::on_openBtn_clicked()
{
    ui->openBtn->setEnabled(false);

    const QString name = ui->portNameCmb->currentText();
    const int baudRate = ui->baudRateCmb->currentText().toInt();

    if(name.isEmpty()){
        QMessageBox::warning(this, "Open Port", "請選擇序列阜的名稱!!");
        ui->openBtn->setEnabled(true);
        return;
    }

    if(openPort(name, baudRate)){
        setPortOpenUi(true);
    }
    else{
        ui->openBtn->setEnabled(true);
    }

}

bool MainWindow::openPort(const QString& portName, const int& baudRate){
    if(serialPort->isOpen())
        serialPort->close();

    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(!serialPort->open(QIODevice::ReadWrite)){
        return false;
    }

    return true;
}

void MainWindow::closePort(){
    if(serialPort->isOpen()){
        serialPort->close();
        setPortOpenUi(false);
    }
}

void MainWindow::sendData(){

    if(!serialPort->isOpen()){
        return;
    }

    QByteArray data = ui->sendDataEdit->text().toUtf8();

    qint64 written = serialPort->write(data);

    if(written == -1){
        QMessageBox::warning(this, "Send Data", "Write failed: "+serialPort->errorString());
        return;
    }
    else{
        appendPacketLog("Tx", data);
    }


}

void MainWindow::readData(){
    const QByteArray data = serialPort->readAll();
    qDebug() << QString::fromUtf8(data);

    if(data.isEmpty()) return;

    rxBuffer.append(data);

    rxTimer->start(20); //只要還有新資料進來，就重新等 20 ms
}

void MainWindow::on_clearLogBtn_clicked()
{
    ui->packetLogTbl->setRowCount(0);
}

void MainWindow::appendPacketLog(const QString& direction, const QByteArray& data){
    int row = ui->packetLogTbl->rowCount(); // 取得目前總列數
    ui->packetLogTbl->insertRow(row); // 在最後新增一列

    QString timeText = QDateTime::currentDateTime().toString("HH:mm:ss.zzzzz");

    ui->packetLogTbl->setItem(row, 0, new QTableWidgetItem(timeText));
    ui->packetLogTbl->setItem(row, 1, new QTableWidgetItem(direction));
    ui->packetLogTbl->setItem(row, 2, new QTableWidgetItem(data));
}

void MainWindow::flushRxBuffer(){
    if(rxBuffer.isEmpty()) return;

    appendPacketLog("Rx", rxBuffer);
    rxBuffer.clear();
}

void MainWindow::handleSerialPortError(QSerialPort::SerialPortError error)
{
    QString title = "";
    QString msg = "Error String: " + serialPort->errorString();

    switch(error)
    {
    case QSerialPort::NoError:
        return;
        break;

    case QSerialPort::PermissionError:
        title = "PermissionError";
        msg += "\n\n請檢查序列阜有沒有被其他程式佔據。";
        break;

    case QSerialPort::ResourceError:
        title = "ResourceError";
        msg += "\n\n裝置可能被拔除或資源異常。";

        setPortOpenUi(false);
        break;
    }

    QMessageBox::warning(this, title, msg);

}
