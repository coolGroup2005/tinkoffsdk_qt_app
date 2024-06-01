#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QString>
#include <QStringListModel>

#include "portfolio.h"
#include "statistics/statistics.h"

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
    void on_listView_clicked(const QModelIndex &index);
    void on_topGainersList_clicked(const QModelIndex &index);
    void on_topLosersList_clicked(const QModelIndex &index);
    void on_topActiveList_clicked(const QModelIndex &index);
    void on_checkBoxStatistics_stateChanged(int state);
    
    void updateStatistics();

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QList<QString> songs;
    Portfolio *portfolio;
    StatisticsManager *statisticsManager;
    DatabaseFigi *databaseFigi;
};

#endif // MAINWINDOW_H
