#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"

#include <QStringList>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    ui->tabWidget->setTabText(1, "Portfolio");
    ui->tabWidget->setTabText(2, "Statistics");
    ui->tabWidget->setTabText(3, "Home");

    // connect(ui->buttonWindow1, &QPushButton::clicked, this, &MainWindow::openDialog);
    QStringList list;
    model = new QStringListModel;

    list << "ITEM 1";
    list << "ITEM 2";
    list << "ITEM 3";
    list << "ITEM 4";
    list << "ITEM 5";
    list << "ITEM 6";
    list << "ITEM 7";
    list << "ITEM 8";
    model->setStringList(list);
    ui->listView->setModel(model);


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

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    ui->lineEdit->setText(";;;  " + selectedItem);
}
