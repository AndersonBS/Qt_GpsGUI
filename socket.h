#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QAbstractSocket>
#include <QByteArray>
#include <QDebug>
#include <QThread>

class Socket: public QObject {
    Q_OBJECT

private:
    QTcpSocket *_socket;
    QTcpServer *_server;
    QString *_socketBuffer;

    void parseSocketBuffer();

public:
    Socket();
    ~Socket();

    void openSocketServer(int port);
    void closeSocketServer();
    int getServerPort();

private slots:
    void on_newConnection();
    void on_disconnected();
    void on_readyRead();

signals:
    void socketReadyRead(QByteArray gpsPackage);

};

#endif // CONNECTION_H
