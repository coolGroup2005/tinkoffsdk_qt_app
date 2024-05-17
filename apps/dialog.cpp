#include "dialog.h"
#include <QStringList>
#include <QStringListModel>
#include "akcii.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

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

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_listView_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    ui->lineEdit->setText(";;;  " + selectedItem);

    Dialog::openAkcii();
}

void Dialog::openAkcii()
{
    akcii *window1 = new akcii(this);
    window1->show();
}
