#include "akcii.h"
#include "ui_akcii.h"

akcii::akcii(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::akcii)
{
    ui->setupUi(this);
}

akcii::~akcii()
{
    delete ui;
}
