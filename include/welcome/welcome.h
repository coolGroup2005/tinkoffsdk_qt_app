#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class Welcome;
}

class Welcome : public QDialog
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

    QString getToken() const;

private slots:
    void on_browseButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::Welcome *ui;
    QString token;

    void loadToken();
    void saveToken();
};

#endif // TWELCOME_H
