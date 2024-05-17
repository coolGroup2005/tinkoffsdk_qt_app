#include "window1.h"
#include "ui_window1.h"

Window1::Window1(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Window1)
{
    ui->setupUi(this);
}

Window1::~Window1()
{
    deleteui;
}
