#ifndef WINDOW2_H
#define WINDOW2_H

#include <QDialog>

namespace Ui {
class Window2;
}

class Window2 : public QDialog
{
    Q_OBJECT

public:
    explicit Window2(QWidget *parent = nullptr);
    ~Window2();

private:
    Ui::Window2 *ui;
};

#endif // WINDOW2_H
