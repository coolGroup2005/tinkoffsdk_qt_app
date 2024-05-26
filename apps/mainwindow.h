#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QString>
#include <QStringListModel>
#include <QStringList>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openAkcii();
    void on_sharesTableView_activated(const QModelIndex &index);
    void on_topGainersList_clicked(const QModelIndex &index);
    void on_topLosersList_clicked(const QModelIndex &index);
    void on_topActiveList_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QList<QString> songs;
};

#endif // MAINWINDOW_H
