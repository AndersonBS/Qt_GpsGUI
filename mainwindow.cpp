#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setCentralWidget(new QWidget);
    this->setStatusBar(new QStatusBar(this));
    QGridLayout *grid = new QGridLayout(this->centralWidget());

    QToolBar *toolbar = this->addToolBar("Main Toolbar");
    QAction *startSerial = toolbar->addAction("START FROM SERIAL");
    QObject::connect(startSerial, SIGNAL(triggered()), this, SLOT(startSerial()));
    QAction *startSocket = toolbar->addAction("START FROM WEB SOCKET");
    QObject::connect(startSocket, SIGNAL(triggered()), this, SLOT(startSocket()));
    QAction *stop = toolbar->addAction("STOP");
    QObject::connect(stop, SIGNAL(triggered()), this, SLOT(stop()));
    QAction *quit = toolbar->addAction("QUIT");
    QObject::connect(quit, SIGNAL(triggered()), this, SLOT(close()));

    _gps = new GPS;
    _serial = NULL;
    _socket = NULL;
    _receivedPackages = 0;

    grid->addWidget(new QLabel("PACKAGE DATE:"), 1, 1);
    _dataPacote = new QLCDNumber(this);
    _dataPacote->setDigitCount(10);
    grid->addWidget(_dataPacote, 1, 2);
    grid->addWidget(new QLabel("PACKAGE TIME:"), 1, 3);
    _horaPacote = new QLCDNumber(this);
    _horaPacote->setDigitCount(8);
    grid->addWidget(_horaPacote, 1, 4);

    grid->addWidget(new QLabel("LATITUDE:"), 2, 1);
    _latitude = new QLCDNumber(this);
    _latitude->setDigitCount(8);
    grid->addWidget(_latitude, 2, 2);
    grid->addWidget(new QLabel("LONGITUDE:"), 2, 3);
    _longitude = new QLCDNumber(this);
    _longitude->setDigitCount(8);
    grid->addWidget(_longitude, 2, 4);

    grid->addWidget(new QLabel("SPEED IN KM/H:"), 3, 1);
    _velocidade = new QLCDNumber(this);
    _velocidade->setDigitCount(6);
    grid->addWidget(_velocidade, 3, 2);
    grid->addWidget(new QLabel("ALTITUDE:"), 3, 3);
    _altitude = new QLCDNumber(this);
    grid->addWidget(_altitude, 3, 4);

    grid->addWidget(new QLabel("Nº OF SATELLITES:"), 4, 1);
    _numeroSatelites = new QLCDNumber(this);
    grid->addWidget(_numeroSatelites, 4, 2);
    grid->addWidget(new QLabel("HORIZONTAL PRECISION:"), 4, 3);
    _precisaoPacote = new QLCDNumber(this);
    grid->addWidget(_precisaoPacote, 4, 4);

    _sateliteList = new QListWidget(this);
    grid->addWidget(_sateliteList, 5, 1, 2, 2);
    _sateliteInfo = new QTextEdit(this);
    _sateliteInfo->setEnabled(false);
    _sateliteInfo->setTextColor(QColor(0, 0, 0));
    grid->addWidget(_sateliteInfo, 5, 3, 2, 2);

    _console = new QTextEdit(this);
    _console->setPalette(QPalette(QColor(0, 0, 0)));
    _console->setTextColor(QColor(0, 255, 0));
    _console->setReadOnly(true);
    grid->addWidget(_console, 7, 1, 3, 4);



    QObject::connect(_sateliteList, SIGNAL(itemSelectionChanged()), this, SLOT(updateSateliteInfo()));
}

MainWindow::~MainWindow() {

}

void MainWindow::showGPS(QByteArray gpsPackage) {
    _gps->parseGPS(gpsPackage);
    if(_gps->_gpgga != NULL) {
        _latitude->display(_gps->_gpgga->_latitude);
        _longitude->display(_gps->_gpgga->_longitude);
        _numeroSatelites->display(_gps->_gpgga->_satelites);
        _altitude->display(_gps->_gpgga->_altitude);
        int hours = (int) _gps->_gpgga->_utcTime / 10000;
        int minutes = (int) _gps->_gpgga->_utcTime / 100 % hours;
        int seconds = (int) _gps->_gpgga->_utcTime % (hours * 100) % minutes;
        _horaPacote->display(QTime(hours, minutes, seconds).toString(QString("hh:mm:ss")));
        _precisaoPacote->display(_gps->_gpgga->_hdop);
    }
    if(_gps->_gprmc != NULL) {
        int day = _gps->_gprmc->_utDate / 10000;
        int month = _gps->_gprmc->_utDate / 100 % (day * 100);
        int year = _gps->_gprmc->_utDate % (day * 100000) % (month * 100) + 2000;
        _dataPacote->display(QDate(year, month, day).toString(QString("dd/MM/yyyy")));
        _velocidade->display(_gps->_gprmc->_speed);
    }
    if(_gps->_gpgsa != NULL) {
        _sateliteList->clear();
        _sateliteInfo->clear();
        for(int index = 0; index < _gps->_gpgsa->_satelites->size(); index++) {
            _sateliteList->addItem("SATELLITE NUMBER " + QString::number(_gps->_gpgsa->_satelites->at(index)));
        }
        if(!_sateliteList->size().isEmpty()) {
            for(int index = 0; index < _gps->_gpgsa->_satelites->size(); index++) {
                if(_selectedSateliteId == _gps->_gpgsa->_satelites->at(index)) {
                    _sateliteList->setCurrentRow(index);
                    break;
                }
            }
        }
    }
    _console->append(gpsPackage);
    this->updateStatusBar();
}

void MainWindow::updateSateliteInfo() {
    if(_gps->_gpgsv != NULL && !_sateliteList->size().isEmpty()) {
        for(int index = 0; index < _gps->_gpgsv->_satelites->size(); index++) {
            if(_sateliteList->selectedItems().size()) {
                _selectedSateliteId = _gps->_gpgsa->_satelites->at(_sateliteList->row(_sateliteList->selectedItems().at(0)));
            }
            if(_gps->_gpgsv->_satelites->at(index)._id == _selectedSateliteId) {
                SATELITE satelite = _gps->_gpgsv->_satelites->at(index);
                _sateliteInfo->setText(QString("ID = " + QString::number(satelite._id) + "\nALTITUDE = " +
                       QString::number(satelite._elevation) + "° (Max. 90°)\n" + "AZIMUTH = " + QString::number(satelite._azimuth) +
                       "° (Max. 359°)\n" + "SNR = " + QString::number(satelite._snr) + " dB\n"));
                break;
            }
        }
    }
}

Serial *MainWindow::setupSerialPortDialog() {
    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Setup Serial Port:"));

    QComboBox *portName = new QComboBox(&dialog);
    QList<QSerialPortInfo> availablePorts = _serial->listSerialPortsAvailable();
    for(int index = 0; index < availablePorts.size(); index++) {
        portName->addItem(availablePorts.at(index).portName(), QString(availablePorts.at(index).portName()));
    }
    form.addRow(QString("Port Name: "), portName);

    QComboBox *baudRate = new QComboBox(&dialog);
    baudRate->addItem(QString::number(QSerialPort::Baud1200), QSerialPort::Baud1200);
    baudRate->addItem(QString::number(QSerialPort::Baud2400), QSerialPort::Baud2400);
    baudRate->addItem(QString::number(QSerialPort::Baud4800), QSerialPort::Baud4800);
    baudRate->addItem(QString::number(QSerialPort::Baud9600), QSerialPort::Baud9600);
    baudRate->addItem(QString::number(QSerialPort::Baud19200), QSerialPort::Baud19200);
    baudRate->addItem(QString::number(QSerialPort::Baud38400), QSerialPort::Baud38400);
    baudRate->addItem(QString::number(QSerialPort::Baud57600), QSerialPort::Baud57600);
    baudRate->addItem(QString::number(QSerialPort::Baud115200), QSerialPort::Baud115200);
    baudRate->setCurrentIndex(7);
    form.addRow(QString("Baud Rate: "), baudRate);

    QComboBox *dataBits = new QComboBox(&dialog);
    dataBits->addItem(QString::number(QSerialPort::Data5), QSerialPort::Data5);
    dataBits->addItem(QString::number(QSerialPort::Data6), QSerialPort::Data6);
    dataBits->addItem(QString::number(QSerialPort::Data7), QSerialPort::Data7);
    dataBits->addItem(QString::number(QSerialPort::Data8), QSerialPort::Data8);
    dataBits->setCurrentIndex(3);
    form.addRow(QString("Data Bits: "), dataBits);

    QComboBox *parity = new QComboBox(&dialog);
    parity->addItem(QString("No Parity"), QSerialPort::NoParity);
    parity->addItem(QString("Even Parity"), QSerialPort::EvenParity);
    parity->addItem(QString("Odd Parity"), QSerialPort::OddParity);
    parity->addItem(QString("Space Parity"), QSerialPort::SpaceParity);
    parity->addItem(QString("Mark Parity"), QSerialPort::MarkParity);
    form.addRow(QString("Parity: "), parity);

    QComboBox *stopBits = new QComboBox(&dialog);
    stopBits->addItem(QString("One"), QSerialPort::OneStop);
    stopBits->addItem(QString("One And a Half"), QSerialPort::OneAndHalfStop);
    stopBits->addItem(QString("Two"), QSerialPort::TwoStop);
    form.addRow(QString("Stop Bits: "), stopBits);

    QComboBox *flowControl = new QComboBox(&dialog);
    flowControl->addItem(QString("No Flow Control"), QSerialPort::NoFlowControl);
    flowControl->addItem(QString("Hardware Control"), QSerialPort::HardwareControl);
    flowControl->addItem(QString("Software Control"), QSerialPort::SoftwareControl);
    form.addRow(QString("Flow Control: "), flowControl);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Accepted) {
        Serial *serial = new Serial;
        if(serial->openSerialPort(portName->itemData(portName->currentIndex()).toString(),
                               static_cast<QSerialPort::BaudRate>(baudRate->itemData(baudRate->currentIndex()).toInt()),
                               static_cast<QSerialPort::DataBits>(dataBits->itemData(dataBits->currentIndex()).toInt()),
                               static_cast<QSerialPort::Parity>(parity->itemData(parity->currentIndex()).toInt()),
                               static_cast<QSerialPort::StopBits>(stopBits->itemData(stopBits->currentIndex()).toInt()),
                               static_cast<QSerialPort::FlowControl>(flowControl->itemData(flowControl->currentIndex()).toInt()))) {
            return serial;
        }
    }
    return NULL;
}

void MainWindow::startSerial() {
    if(_serial == NULL) {
        _serial = this->setupSerialPortDialog();
        if(_serial != NULL) {
            QObject::connect(_serial, SIGNAL(serialReadyRead(QByteArray)), this, SLOT(showGPS(QByteArray)));
        }
    }
    this->updateStatusBar();
}

void MainWindow::startSocket() {
    if(_socket == NULL) {
        QInputDialog *inputDialog = new QInputDialog(this);
        bool ok;
        int port = inputDialog->getInt(this, QString("WEB SOCKET"), QString("Choose server port: "), 6000, 0, 65535, 1, &ok);
        if(ok) {
            _socket = new Socket;
            _socket->openSocketServer(port);
            QObject::connect(_socket, SIGNAL(socketReadyRead(QByteArray)), this, SLOT(showGPS(QByteArray)));
        }
    }
    this->updateStatusBar();
}

void MainWindow::stop() {
    if(_serial != NULL) {
        delete _serial;
        _serial = NULL;
    }
    if(_socket != NULL) {
        delete _socket;
        _socket = NULL;
    }
    this->updateStatusBar();
}

void MainWindow::updateStatusBar() {
    if(_serial == NULL && _socket == NULL) {
        this->statusBar()->showMessage(QString("DISCONNECTED"));
    } else {
        QString str = QString("CONNECTED ON: ");
        if(_serial != NULL) {
            str.append(QString("Serial ") + _serial->getSerialPortName() + QString(" at ") +
                        QString::number(_serial->getSerialPortBaudRate()) + QString(" bps; "));
        }
        if(_socket != NULL) {
            str.append(QString("Web Socket on port ") + QString::number(_socket->getServerPort()) + "; ");
        }
        str.append(QString("PACKAGES RECEIVED: ") + QString::number(++_receivedPackages));
        this->statusBar()->showMessage(str);
    }
}
