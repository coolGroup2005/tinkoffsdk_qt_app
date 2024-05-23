#include <QApplication>
#include "mainwindow.h"
// #include "invest/example.h"
#include "homepage/homepage.h"


int main(int argc, char *argv[])
{
    MyInvest mi;
    mi.getCandles();
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
