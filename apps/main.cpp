// main.cpp
#include <QApplication>
#include "mainwindow.h"
#include <iostream>
#include "invest/example.h"


int main(int argc, char *argv[])
{
    std::cout << "OKKKKKK!";
    MyInvest mi;
    mi.getCandles();
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
