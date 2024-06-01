#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "welcome/welcome.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
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
