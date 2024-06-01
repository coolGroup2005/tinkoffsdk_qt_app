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
    statisticsManager(new StatisticsManager(this))
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    ui->tabWidget->setTabText(1, "Statistics");
    ui->tabWidget->setTabText(2, "Home");

    portfolio = new Portfolio(this);
    ui->tabWidget->addTab(portfolio, "Portfolio");

    // Iteraction with tab Home
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

    connect(ui->listView, &QListView::activated, this, &MainWindow::on_listView_clicked);

    // Connect list view clicked signals to slots
    connect(ui->top_gainers_list, &QListView::clicked, this, &MainWindow::on_topGainersList_clicked);
    connect(ui->top_losers_list, &QListView::clicked, this, &MainWindow::on_topLosersList_clicked);
    connect(ui->top_active_list, &QListView::clicked, this, &MainWindow::on_topActiveList_clicked);

    connect(ui->updateStatisticsButton, &QPushButton::clicked, this, &MainWindow::updateStatistics);


    // Interaction with tab Statistics ===============================================================
}

void MainWindow::updateStatistics()
{
    QStringListModel *topGainersModel = new QStringListModel(this);
    QStringListModel *topLosersModel = new QStringListModel(this);
    QStringListModel *topActiveModel = new QStringListModel(this);

    ui->intervalStatisticsCombobox->addItem("1 day");
    ui->intervalStatisticsCombobox->addItem("1 week");
    ui->intervalStatisticsCombobox->addItem("1 month");

    QString intervalTextStatistics = ui->intervalStatisticsCombobox->currentText();
    int intervalToPass = (intervalTextStatistics == "1 day") ? 0 : (intervalTextStatistics == "1 month") ? 1 : 2;

    statisticsManager->updateStatistics(intervalToPass, topGainersModel, topLosersModel, topActiveModel);

    ui->top_gainers_list->setModel(topGainersModel);
    ui->top_losers_list->setModel(topLosersModel);
    ui->top_active_list->setModel(topActiveModel);
}

// void MainWindow::updateStatistics() {
//     QStringListModel *topGainersModel = new QStringListModel(this);
//     QStringListModel *topLosersModel = new QStringListModel(this);
//     QStringListModel *topActiveModel = new QStringListModel(this);

//     ui->intervalStatisticsCombobox->addItem("1 day");
//     ui->intervalStatisticsCombobox->addItem("1 week");
//     ui->intervalStatisticsCombobox->addItem("1 month");

//     QString intervalTextStatistics = ui->intervalStatisticsCombobox->currentText();
//     int intervalToPass;
//     if (intervalTextStatistics == "1 day") {
//         intervalToPass = 0;
//     } else if (intervalTextStatistics == "1 month") {
//         intervalToPass = 1;
//     } else {
//         intervalToPass = 2;
//     }


//     InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

//     auto allShares = getAllSharesWithChange(client, intervalToPass);
//     insertStatisticsIntoDatabase(allShares);
//     std::vector<std::pair<std::string, float>> top = getTopFromDb("ASC");

//     QStringList topGainers;

//     for (const auto& sharePair : top) {
//         std::cout << "Company: " << sharePair.first << "\n"
//                 << "Price Change: " << sharePair.second << "%\n"
//                 << "-----------------------------\n";
//     }

//     for (const auto& sharePair : top) {
//         QString gainer = QString::fromStdString(sharePair.first) + " (" + QString::number(sharePair.second) + ")";
//         topGainers.append(gainer);
//     }

//     // Example data for the statistics lists
//     // QStringList topGainers = {"Gainer 1", "Gainer 2", "Gainer 3"};
//     QStringList topLosers = {"Loser 1", "Loser 2", "Loser 3"};
//     QStringList topActive = {"Active 1", "Active 2", "Active 3"};

    
// }

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


void MainWindow::on_topActiveList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    QMessageBox::information(this, "Top Active Selected", "You selected: " + selectedItem);
}


// END =====================================================================================


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
