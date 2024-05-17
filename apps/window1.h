#ifndef WINDOW1_H
#define WINDOW1_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Window1;
}
QT_END_NAMESPACE

class Window1 : public QDialog
{
    Q_OBJECT

public:
    Window1(QWidget *parent = nullptr);
    ~Window1();

private:
    Ui::Window1 *ui;
};

#endif // WINDOW1_H
