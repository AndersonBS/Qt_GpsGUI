#include "serial.h"

Serial::Serial(): QObject() {
    _serialPort = new QSerialPort(this);
    _serialPortInfo = new QSerialPortInfo();
    _serialBuffer = new QString;

    connect(_serialPort, SIGNAL(readyRead()), this, SLOT(readSerial()));
    connect(_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
}

Serial::~Serial() {
    this->closeSerialPort();
    delete _serialPort;
    delete _serialPortInfo;
    delete _serialBuffer;
}

void Serial::openSerialPort(QString portName,
                            QSerialPort::BaudRate baudRate,
                            QSerialPort::DataBits dataBits,
                            QSerialPort::Parity parity,
                            QSerialPort::StopBits stopBits,
                            QSerialPort::FlowControl flowControl) {
    this->closeSerialPort();
    _serialPort->setPortName(portName);
    _serialPort->setBaudRate(baudRate);
    _serialPort->setDataBits(dataBits);
    _serialPort->setParity(parity);
    _serialPort->setStopBits(stopBits);
    _serialPort->setFlowControl(flowControl);
    if (_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened!";
    } else {
        qDebug() << "Could not open Serial!";
    }
}

void Serial::closeSerialPort() {
    if (_serialPort->isOpen()) {
        _serialPort->close();
        qDebug() << "Serial port closed!";
    }
}

void Serial::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        this->closeSerialPort();
        qDebug() << "ERROR: Serial port Closed!";
    }
}

void Serial::readSerial() {
    _serialBuffer->append(_serialPort->readAll());
    if(_serialBuffer->size()) {
        this->parseSerialBuffer();
    }
}

void Serial::parseSerialBuffer() {
    if(_serialBuffer->left(6).compare("$GPGGA") == 0) {
        unsigned char subPackages = 0;
        for(int index = 0; index < _serialBuffer->size(); index++) {
            if(_serialBuffer->at(index) == '\n') {
                subPackages++;
                if(subPackages == 6) {
                    emit serialReadyRead(_serialBuffer->left(index + 1).toLocal8Bit());
                    _serialBuffer->remove(0, index + 1);
                }
            }
        }
    } else {
        qDebug() << "Removing incomplete package from serial buffer!";
        while(_serialBuffer->left(6).compare("$GPGGA") && _serialBuffer->size()) {
            _serialBuffer->remove(0, 1);
        }
    }
}

QList<QSerialPortInfo> Serial::listSerialPortsAvailable() {
    return _serialPortInfo->availablePorts();
}

QString Serial::getSerialPortName() {
    return _serialPort->portName();
}

int Serial::getSerialPortBaudRate() {
    return _serialPort->baudRate();
}
