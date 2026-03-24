#ifndef SERIALPORTDEVICE_H
#define SERIALPORTDEVICE_H

#include <Qstring>

class SerialPortDevice
{
private:
    QString portName;
    QString baudRate;

public:
    SerialPortDevice();
};

#endif // SERIALPORTDEVICE_H
