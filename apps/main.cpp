#include "statistics/statistics.h"
#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"
#include <ordersservice.h> 
#include <QApplication>
#include "mainwindow.h"
#include "marketdataservice.h"
#include "instrumentsservice.h"

#include <signal.h>
#include <QMessageBox>


void manageSegFailure(int);


int main(int argc, char *argv[])
{
    QString scrollBarStyle = R"(
        QScrollBar::up-arrow, QScrollBar::down-arrow {
            width: 0px;
            height: 0px;
            }
        QScrollBar:vertical { 
            background: transparent; 
            width: 10px; 
            margin: 0px 0px 0px 0px; 
            }
        QScrollBar::add-line:vertical { 
            border: 0px; 
            height: 0px; 
            }
        QScrollBar::sub-line:vertical { 
            border: 0px; 
            height: 0px; 
            }
        QScrollBar:horizontal { 
            background: transparent; 
            width: 0px; 
            margin: 0px 0px 0px 0px; 
            }
    )";

    QApplication app(argc, argv);
    app.setStyleSheet(scrollBarStyle);

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