#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPort(new QSerialPort(this))

{
    ui->setupUi(this);

    initWidgets();

    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &MainWindow::loadPorts);

    connect(ui->portNameCmb, &QComboBox::currentIndexChanged,
            this, &MainWindow::showDeviceInfo);

    connect(ui->closeBtn, &QPushButton::clicked,
            this, &MainWindow::closePort);

    connect(ui->sendBtn, &QPushButton::clicked,
            this, &MainWindow::sendData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidgets(){

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
    qDebug() << "Item Changed !!";
    qDebug() << index;

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
    qDebug() << name;
    qDebug() << baudRate;

    if(name.isEmpty()){
        QMessageBox::warning(this, "Open Port", "請選擇序列阜的名稱!!");
        ui->openBtn->setEnabled(true);
        return;
    }

    if(openPort(name, baudRate)){
        ui->closeBtn->setEnabled(true);
        ui->sendBtn->setEnabled(true);
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
        QMessageBox::warning(this,"Open Port","開啟失敗: "+serialPort->errorString());
        return false;
    }

    return true;
}

void MainWindow::closePort(){
    if(serialPort->isOpen()){
        serialPort->close();
        ui->openBtn->setEnabled(true);
        ui->closeBtn->setEnabled(false);
        ui->sendBtn->setEnabled(false);
    }
}

void MainWindow::sendData(){

    if(!serialPort->isOpen()) return;

    QByteArray data = ui->sendDataEdit->text().toUtf8();

    serialPort->write(data);

    appendPacketLog("Tx", data);

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


