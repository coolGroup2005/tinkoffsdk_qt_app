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
#include <QListWidget>
#include <vector>
#include "favouritesModel.h"
#include "favouritesProxyModel.h"
#include "portfolio.h"

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
    void openShares(const std::string& figi, const std::string& stockName);
    void on_sharesTableView_doubleClicked(const QModelIndex &index);
    void on_topGainersList_clicked(const QModelIndex &index);
    void on_topLosersList_clicked(const QModelIndex &index);
    void on_topActiveList_clicked(const QModelIndex &index);

    void createCheckboxList();
    void connectCheckboxes();
    void checkItemsChecked(QListWidgetItem*);
    void updateFilter();

private:
    Ui::MainWindow *ui;
    FavouritesModel *favouritesModel;
    ProxyModel *proxyModel;
    Portfolio *portfolio;
};

#endif // MAINWINDOW_H
