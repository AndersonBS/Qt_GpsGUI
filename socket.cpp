#include "socket.h"

Socket::Socket() {
    _server = NULL;
    _socket = NULL;
    _socketBuffer = new QString;
}

Socket::~Socket() {
    if(_socket != NULL) {
        _socket->close();
        delete _socket;
        _socket = NULL;
    }
    if(_server != NULL) {
        this->closeSocketServer();
    }
}

void Socket::openSocketServer(int port) {
    this->closeSocketServer();
    _server = new QTcpServer(this);
    _server->listen(QHostAddress::Any, port);
    connect(_server, SIGNAL(newConnection()), this, SLOT(on_newConnection()));
    qDebug() << "Socket server started!";
}

void Socket::closeSocketServer() {
    if(_server != NULL) {
        _server->close();
        delete _server;
        _server = NULL;
        qDebug() << "Socket server closed!";
    }
}

void Socket::on_newConnection() {
    if(_socket != NULL) {
        _socket->close();
        qDebug() << "Connection closed!";
    }
    _socket = _server->nextPendingConnection();

    if(_socket->state() == QTcpSocket::ConnectedState) {
        qDebug() << "New connection established.";
        connect(_socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
        connect(_socket, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    }

    for(int x = 0; x < 3; x++) {
        _socket->write("#OK;");
        _socket->waitForBytesWritten(-1);
        _socket->flush();
        QThread::msleep(1000);
    }

}

void Socket::on_disconnected() {
    _socket->close();
    _socket = NULL;
    qDebug() << "Connection closed!";
}

void Socket::on_readyRead() {
    _socketBuffer->append(_socket->readAll());
    if(_socketBuffer->size()) {
        this->parseSocketBuffer();
    }
}

void Socket::parseSocketBuffer() {
    if(_socketBuffer->left(6).compare("$GPGGA") == 0) {
        unsigned char subPackages = 0;
        for(int index = 0; index < _socketBuffer->size(); index++) {
            if(_socketBuffer->at(index) == '\n') {
                subPackages++;
                if(subPackages == 6) {
                    emit socketReadyRead(_socketBuffer->left(index + 1).toLocal8Bit());
                    _socketBuffer->remove(0, index + 1);
                }
            }
        }
    } else {
        qDebug() << "Removing incomplete package from socket buffer!";
        while(_socketBuffer->left(6).compare("$GPGGA") && _socketBuffer->size()) {
            _socketBuffer->remove(0, 1);
        }
    }
}

int Socket::getServerPort() {
    return _server->serverPort();
}
