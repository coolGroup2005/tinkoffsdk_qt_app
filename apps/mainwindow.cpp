#include "mainwindow.h"
#include "shares/shares.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"

#include "portfolio.h"
#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>

#include <vector>

#include <QListWidget>

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
    createCheckboxList();
    ui->sharesTableView->setModel(proxyModel);
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


void MainWindow::createCheckboxList()
{
    QStringList  itemLabels = {"STATUS UNSPECIFIED", "NOT AVAILABLE FOR TRADING", 
                            "OPENING PERIOD", "CLOSING PERIOD", "BREAK IN TRADING", 
                            "NORMAL TRADING", "CLOSING AUCTION", "DARK POOL AUCTION", 
                            "DISCRETE AUCTION", "OPENING AUCTION PERIOD", "TRADING AT CLOSING AUCTION PRICE", 
                            "SESSION ASSIGNED", "SESSION CLOSE", "SESSION OPEN", 
                            "DEALER NORMAL TRADING", "DEALER BREAK IN TRADING", "DEALER NOT AVAILABLE FOR TRADING"};

    QStringListIterator it(itemLabels);
    while (it.hasNext())
    {
        QListWidgetItem *listItem = new QListWidgetItem(it.next(), ui->listWidget);
        listItem->setCheckState(Qt::Unchecked);
        listItem->setFlags(listItem->flags() & ~Qt::ItemIsSelectable);
        ui->listWidget->addItem(listItem);
    }

    connectCheckboxes();
}


void MainWindow::connectCheckboxes() 
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        connect(ui->listWidget, &QListWidget::itemChanged, this, &MainWindow::checkItemsChecked);
    }
}


void MainWindow::checkItemsChecked(QListWidgetItem* changedItem)
{
    if (changedItem->checkState() == Qt::Checked) 
    {
        for (int i = 0; i < ui->listWidget->count(); ++i) 
        {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (item != changedItem) 
                item->setCheckState(Qt::Unchecked);
        }
        updateFilter();
    }
    else
        proxyModel->setTradingStatus("ALL");
}

void MainWindow::updateFilter() 
{
    QString filterString;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            filterString = item->text();
        }
    }
    proxyModel->setTradingStatus(filterString);
}


void MainWindow::on_sharesTableView_doubleClicked(const QModelIndex &index)
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


