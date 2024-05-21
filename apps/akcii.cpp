#include "akcii.h"
#include "ui_akcii.h"

akcii::akcii(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::akcii)
{
    ui->setupUi(this);
    this->resize(840, 520);
    this->setFixedSize(this->size()); //lock an ability to change the size

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &akcii::updateDateTime);
    timer->start(1000);
}
akcii::~akcii()
{
    delete ui;
}

void akcii::updateDateTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->dateLabel->setText(currentDateTime.toString("dddd, MMMM d"));
    ui->timeLabel->setText(currentDateTime.toString("h:mm:ss"));
}
