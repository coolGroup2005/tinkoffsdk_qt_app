#ifndef AKCII_H
#define AKCII_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class akcii;
}

class akcii : public QDialog
{
    Q_OBJECT

public:
    explicit akcii(QWidget *parent = nullptr);
    ~akcii();

private slots:
    void updateDateTime();

private:
    Ui::akcii *ui;
    QTimer *timer;
};

#endif // AKCII_H
