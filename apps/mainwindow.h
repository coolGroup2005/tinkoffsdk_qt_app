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
#include <QStringList>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QListWidget>
#include <vector>
#include "portfolio.h"
#include "figi.h"

#include "mainwindow.h"
#include "shares/shares.h"
#include "statistics/statistics.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"
#include "portfolio.h"
#include "figi.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class DatabaseFigi;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, const QString& token = "");
    ~MainWindow();
    void openShares(const std::string& figi, const std::string& stockName, QString token);


private slots:
    void on_listView_clicked(const QModelIndex &index);
    void on_sharesTableView_doubleClicked(const QModelIndex &index);
    void on_topGainersList_clicked(const QModelIndex &index);
    void on_topLosersList_clicked(const QModelIndex &index);
    void updateStatistics();

    void createCheckboxList();
    void connectCheckboxes();
    void checkItemsChecked(QListWidgetItem*);
    void updateFilter();

private:
    Ui::MainWindow *ui;
    FavouritesModel *favouritesModel;
    ProxyModel *proxyModel;
    Portfolio *portfolio;
    StatisticsManager *statisticsManager;
    DatabaseFigi *databaseFigi;
    QString token;
};

#endif // MAINWINDOW_H
