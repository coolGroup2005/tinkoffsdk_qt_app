#ifndef AKCII_H
#define AKCII_H

#include <QDialog>

namespace Ui {
class akcii;
}

class akcii : public QDialog
{
    Q_OBJECT

public:
    explicit akcii(QWidget *parent = nullptr);
    ~akcii();

private:
    Ui::akcii *ui;
};

#endif // AKCII_H
