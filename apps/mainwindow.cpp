#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    ui->tabWidget->setTabText(1, "Portfolio");
    ui->tabWidget->setTabText(2, "Statistics");
    ui->tabWidget->setTabText(3, "Home");

    connect(ui->buttonWindow1, &QPushButton::clicked, this, &MainWindow::openDialog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDialog()
{
    Dialog *window1 = new Dialog(this);
    window1->show();
}
