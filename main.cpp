#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QTcpSocket>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

    MainWindow *w = new MainWindow;
    w->setMinimumSize(880, 660);
    w->show();

    return a.exec();
}
