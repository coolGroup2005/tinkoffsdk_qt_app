#include "mainwindow.h"
#include "shares/shares.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"

#include "portfolio.h"
#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>

#include <vector>

// QStandardItemModel* fillSharesList()
// {
//     std::vector<ShareInfo> sharesList;
//     sharesList = parseFigi();

//     const int numRows = sharesList.size();
//     const int numColumns = 3;
//     QStandardItemModel* model = new QStandardItemModel(numRows, numColumns);
//     model->setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);
//     model->setHeaderData(1, Qt::Horizontal, "Figi", Qt::DisplayRole);
//     model->setHeaderData(2, Qt::Horizontal, "Trading Status", Qt::DisplayRole);
//     for (int i = 0; i < numRows; ++i)
//     {
//         QList<QStandardItem*> itemsList;
//         itemsList.append(new QStandardItem(QString::fromStdString(sharesList[i].name)));
//         itemsList.append(new QStandardItem(QString::fromStdString(sharesList[i].figi)));
//         itemsList.append(new QStandardItem(QString::fromStdString(formatTradingStatus(sharesList[i].trading_status))));
//         for (short j = 0; j < 3; ++j)
//         {
//             itemsList[j]->setEditable(false);
//             model->setItem(i, j, itemsList[j]);
//         }
//     }
//     return model;
// }


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , favouritesModel(new FavouritesModel)
    , proxyModel(new ProxyModel)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    ui->tabWidget->setTabText(1, "Statistics");
    ui->tabWidget->setTabText(2, "Home");

    portfolio = new Portfolio(this);
    ui->tabWidget->addTab(portfolio, "Portfolio");

    // Iteraction with tab Home
    proxyModel->setSourceModel(favouritesModel);
    // TODO: implement getting filter from user
    proxyModel->setTradingStatus("SESSION OPEN");

    ui->sharesTableView->setModel(proxyModel);
    // ui->sharesTableView->setSortingEnabled(true); 
    ui->sharesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->sharesTableView->verticalHeader()->setVisible(false);
    ui->sharesTableView->setStyleSheet("QHeaderView::section {background-color: lightgrey}");

    ui->labelAccount->setText(accountsInfoText());
    ui->labelAccount->setStyleSheet("QLabel {"
                             "border-style: solid;"
                             "border-width: 1px;"
                             "border-color: rgba(60, 60, 60, 0.8); "
                             "}");

    // Interaction with tab Statistics
    // Initialize models for the statistics lists
    QStringListModel *topGainersModel = new QStringListModel(this);
    QStringListModel *topLosersModel = new QStringListModel(this);
    QStringListModel *topActiveModel = new QStringListModel(this);

    // Example data for the statistics lists
    QStringList topGainers = {"Gainer 1", "Gainer 2", "Gainer 3"};
    QStringList topLosers = {"Loser 1", "Loser 2", "Loser 3"};
    QStringList topActive = {"Active 1", "Active 2", "Active 3"};

    topGainersModel->setStringList(topGainers);
    topLosersModel->setStringList(topLosers);
    topActiveModel->setStringList(topActive);

    // Set models for the statistics list views
    ui->top_gainers_list->setModel(topGainersModel);
    ui->top_losers_list->setModel(topLosersModel);
    ui->top_active_list->setModel(topActiveModel);

    ui->top_gainers_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->top_losers_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->top_active_list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    // Connect list view clicked signals to slots
    connect(ui->top_gainers_list, &QListView::clicked, this, &MainWindow::on_topGainersList_clicked);
    connect(ui->top_losers_list, &QListView::clicked, this, &MainWindow::on_topLosersList_clicked);
    connect(ui->top_active_list, &QListView::clicked, this, &MainWindow::on_topActiveList_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sharesTableView_activated(const QModelIndex &index)
{
    QString stockName = ui->sharesTableView->model()->index(index.row(),0).data().toString();
    QString figi = ui->sharesTableView->model()->index(index.row(),1).data().toString();

    ui->lineEdit->setText(stockName);

    MainWindow::openShares(figi.toStdString(), stockName.toStdString());
}


void MainWindow::openShares(const std::string& figi, const std::string& stockName)
{
    shares *window1 = new shares(this, figi, stockName);
    window1->show();
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


void MainWindow::on_topActiveList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    QMessageBox::information(this, "Top Active Selected", "You selected: " + selectedItem);
}


