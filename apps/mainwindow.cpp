#include "mainwindow.h"
#include "shares/shares.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"
#include "statistics/statistics.h"
#include "portfolio.h"

#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>

#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    statisticsManager(new StatisticsManager(this)),
    databaseFigi(new DatabaseFigi(this))
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Statistics");
    ui->tabWidget->setTabText(1, "Home");

    portfolio = new Portfolio(this);
    ui->tabWidget->addTab(portfolio, "Portfolio");
    ui->tabWidget->addTab(databaseFigi, "Database Figi");


    // Iteraction with tab Home ====================================================
    QStringList list;
    model = new QStringListModel;

    std::vector<ShareInfo> sharesList;
    sharesList = parseFigi();
    
    for (ShareInfo availableShare: sharesList)
    {
        QString listItem = QString::fromStdString(availableShare.name + "\t" + availableShare.figi + "\t" + formatTradingStatus(availableShare.trading_status));
        list << listItem;
    }

    model->setStringList(list);
    ui->listView->setModel(model);
    // END Home ===================================================================


    // Interaction with tab Statistics ============================================
    ui->checkBoxStatistics->setChecked(true);

    connect(ui->listView, &QListView::activated, this, &MainWindow::on_listView_clicked);
    connect(ui->top_gainers_list, &QListView::clicked, this, &MainWindow::on_topGainersList_clicked);
    connect(ui->top_losers_list, &QListView::clicked, this, &MainWindow::on_topLosersList_clicked);
    connect(ui->updateStatisticsButton, &QPushButton::clicked, this, &MainWindow::updateStatistics);

    ui->intervalStatisticsCombobox->addItem("1 day");
    ui->intervalStatisticsCombobox->addItem("1 week");
    ui->intervalStatisticsCombobox->addItem("1 month");

    databaseFigi->insertSharesIntoDatabase();
    updateStatistics();
    // END Statistics ================================================================
}


void MainWindow::updateStatistics()
{
    QStringListModel *topGainersModel = new QStringListModel(this);
    QStringListModel *topLosersModel = new QStringListModel(this);
    QStringListModel *topActiveModel = new QStringListModel(this);


    QString intervalTextStatistics = ui->intervalStatisticsCombobox->currentText();
    int intervalToPass = (intervalTextStatistics == "1 day") ? 0 : (intervalTextStatistics == "1 month") ? 1 : 2;

    bool cropped = ui->checkBoxStatistics->isChecked();
    statisticsManager->updateStatistics(intervalToPass, topGainersModel, topLosersModel, topActiveModel, cropped);

    QStringList topLosers = {"Press UPD Button and wait a little"};
    QStringList topActive = {"Press UPD Button and wait a little"};

    ui->top_gainers_list->setModel(topGainersModel);
    ui->top_losers_list->setModel(topLosersModel);
}


void MainWindow::on_topGainersList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    QMessageBox::information(this, "Top Gainer Selected", "You selected: " + selectedItem);
}

void MainWindow::on_topLosersList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    QMessageBox::information(this, "Top Loser Selected", "You selected: " + selectedItem);
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    ui->lineEdit->setText(";;;  " + selectedItem);
    std::string figi = selectedItem.split("\t")[1].toStdString(); 
    std::string stockName = selectedItem.section('\t', 0, 0).toStdString();   
    std::cout << figi;
    MainWindow::openShares(figi, stockName);
}


void MainWindow::openShares(const std::string& figi, const std::string& stockName)
{
    shares *window1 = new shares(this, figi, stockName);
    window1->show();
}
