#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QTextEdit>
#include <QObject>
#include <QListWidget>
#include <QTime>
#include <QDate>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QInputDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QVariant>

#include "gps.h"
#include "serial.h"
#include "socket.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    GPS *_gps;
    Serial *_serial;
    Socket *_socket;

    QLCDNumber *_dataPacote;
    QLCDNumber *_horaPacote;

    QLCDNumber *_latitude;
    QLCDNumber *_longitude;

    QLCDNumber *_velocidade;
    QLCDNumber *_altitude;

    QLCDNumber *_numeroSatelites;
    QLCDNumber *_precisaoPacote;

    QListWidget *_sateliteList;
    QTextEdit *_sateliteInfo;

    QTextEdit *_console;

    int _selectedSateliteId;
    int _receivedPackages;

    Serial *setupSerialPortDialog();
    void updateStatusBar();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateSateliteInfo();
    void showGPS(QByteArray gpsPackage);
    void startSerial();
    void startSocket();
    void stop();

};

#endif // MAINWINDOW_H
