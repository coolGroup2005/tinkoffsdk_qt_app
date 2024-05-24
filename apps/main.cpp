#include <QApplication>
#include "mainwindow.h"
// #include "invest/example.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
