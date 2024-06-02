#include "welcome/welcome.h"
#include "ui_welcome.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

Welcome::Welcome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    loadToken();
}

Welcome::~Welcome()
{
    delete ui;
}

QString Welcome::getToken() const
{
    return token;
}

void Welcome::on_browseButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Token File"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString fileToken = in.readLine();
            ui->tokenLineEdit->setText(fileToken);
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Unable to open file."));
        }
    }
}

void Welcome::on_saveButton_clicked()
{
    token = ui->tokenLineEdit->text();
    saveToken();
    accept();
}

void Welcome::loadToken()
{
    QSettings settings("YourCompany", "YourApp");
    token = settings.value("token", "").toString();
    ui->tokenLineEdit->setText(token);
}

void Welcome::saveToken()
{
    QSettings settings("YourCompany", "YourApp");
    settings.setValue("token", token);
}
