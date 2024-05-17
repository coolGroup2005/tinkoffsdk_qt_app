#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QMainWindow>
#include <QString>
#include <QStringListModel>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QStringListModel *model;
    QList<QString> songs;
private slots:
    void on_listView_clicked(const QModelIndex &index);
    void openAkcii();
};

#endif // DIALOG_H
