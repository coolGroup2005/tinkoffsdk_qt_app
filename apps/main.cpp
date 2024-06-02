#include "statistics/statistics.h"
#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"
#include <ordersservice.h> 
#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "marketdataservice.h"
#include "instrumentsservice.h"
#include "welcome/welcome.h"
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
    
    Welcome tokenDialog;
    if (tokenDialog.exec() == QDialog::Accepted) {
        QString token = tokenDialog.getToken();

        if (token.isEmpty() || token.length() <= 80) {
            QMessageBox::critical(nullptr, "Error", "Invalid or empty token provided. Please provide a valid token.");
            return 1;
        }
        MainWindow mainWindow(nullptr, token);
        mainWindow.show();

        return app.exec();
    }
    
    return 0;

}


void manageSegFailure(int signalCode)
{
    QMessageBox::critical(nullptr, "Error", "Access denied\nCheck access level of token in settings", QMessageBox::Ok);
    signal(signalCode, SIG_DFL);
    QApplication::exit(3);
}