#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>


class Serial: public QObject {
    Q_OBJECT
private:
    QSerialPort *_serialPort;
    QSerialPortInfo *_serialPortInfo;
    QString *_serialBuffer;

    void parseSerialBuffer();

public:


    Serial();
    ~Serial();
    void openSerialPort(QString portName,
                        QSerialPort::BaudRate baudRate = QSerialPort::Baud115200,
                        QSerialPort::DataBits dataBits = QSerialPort::Data8,
                        QSerialPort::Parity parity = QSerialPort::NoParity,
                        QSerialPort::StopBits stopBits = QSerialPort::OneStop,
                        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl);
    void closeSerialPort();
    QList<QSerialPortInfo> listSerialPortsAvailable();
    QString getSerialPortName();
    int getSerialPortBaudRate();

signals:
    void serialReadyRead(QByteArray gpsPackage);

private slots:
    void handleSerialError(QSerialPort::SerialPortError error);
    void readSerial();
};

#endif // SERIAL_H
