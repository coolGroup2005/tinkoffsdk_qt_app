#include <QApplication>
#include "mainwindow.h"
// #include "invest/example.h"

#include <signal.h>
#include <QMessageBox>


void manageSegFailure(int);


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    signal(SIGSEGV, manageSegFailure);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}


void manageSegFailure(int signalCode)
{
    QMessageBox::critical(nullptr, "Error", "Access denied\nCheck access level of token in settings", QMessageBox::Ok);
    signal(signalCode, SIG_DFL);
    QApplication::exit(3);
}