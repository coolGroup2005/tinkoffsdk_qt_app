#include "mainwindow.h"
#include "akcii.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"
#include "portfolio.h"

#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>

#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    // ui->tabWidget->setTabText(1, "Portfolio");
    ui->tabWidget->setTabText(2, "Statistics");
    ui->tabWidget->setTabText(3, "Home");

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

    // Connect listView click signal to slot
    connect(ui->listView, &QListView::activated, this, &MainWindow::on_listView_clicked);

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


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    ui->lineEdit->setText(";;;  " + selectedItem);
    std::string figi = selectedItem.split("\t")[1].toStdString(); 
    std::string stockName = selectedItem.section('\t', 0, 0).toStdString();   
    std::cout << figi;
    MainWindow::openAkcii(figi, stockName);
}


void MainWindow::openAkcii(const std::string& figi, const std::string& stockName)
{
    akcii *window1 = new akcii(this, figi, stockName);
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
