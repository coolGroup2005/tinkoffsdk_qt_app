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

MainWindow::MainWindow(QWidget *parent, const QString& token)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , favouritesModel(new FavouritesModel(this, token))
    , proxyModel(new ProxyModel)
    , statisticsManager(new StatisticsManager(this))
    , databaseFigi(new DatabaseFigi(this))
    , token(token)
{
    ui->setupUi(this);
    this->resize(850, 600);
    this->setMinimumSize(this->size());
    ui->tabWidget->setTabText(0, "Home");
    ui->tabWidget->setTabText(1, "Statistics");

    portfolio = new Portfolio(this, token);
    ui->tabWidget->addTab(portfolio, "Portfolio");
    ui->tabWidget->addTab(databaseFigi, "Database Figi");

    // Iteraction with tab Home ===================================================
    proxyModel->setSourceModel(favouritesModel);
    createCheckboxList();
    ui->sharesTableView->setModel(proxyModel);
    ui->sharesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->sharesTableView->verticalHeader()->setVisible(false);
    ui->sharesTableView->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #ebeae7;
            border-top: 0px solid #b0b0af;
            border-bottom: 0px solid #b0b0af;
            border-right: 1px solid #b0b0af;
        }
        QHeaderView::section:horizontal:first {
            border-left: 1px solid #b0b0af;
        }
        QTableView {
            gridline-color: #b0b0af;
            background-color: rgb(222, 222, 222);
            border-radius: 20px;
        }
    )");
    // END Home ===================================================================
    // Interaction with tab Statistics ============================================
    ui->checkBoxStatistics->setChecked(true);

    // connect(ui->listView, &QListView::activated, this, &MainWindow::on_listView_clicked);
    connect(ui->top_gainers_list, &QListView::clicked, this, &MainWindow::on_topGainersList_clicked);
    connect(ui->top_losers_list, &QListView::clicked, this, &MainWindow::on_topLosersList_clicked);
    connect(ui->updateStatisticsButton, &QPushButton::clicked, this, &MainWindow::updateStatistics);

    ui->intervalStatisticsCombobox->addItem("1 day");
    ui->intervalStatisticsCombobox->addItem("1 week");
    ui->intervalStatisticsCombobox->addItem("1 month");




    databaseFigi->insertSharesIntoDatabase(token);
    // updateStatistics();
    // END Statistics ================================================================

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateStatistics()
{
    QStringListModel *topGainersModel = new QStringListModel(this);
    QStringListModel *topLosersModel = new QStringListModel(this);
    QStringListModel *topActiveModel = new QStringListModel(this);


    QString intervalTextStatistics = ui->intervalStatisticsCombobox->currentText();
    int intervalToPass = (intervalTextStatistics == "1 day") ? 0 : (intervalTextStatistics == "1 month") ? 1 : 2;

    bool cropped = ui->checkBoxStatistics->isChecked();
    statisticsManager->updateStatistics(token, intervalToPass, topGainersModel, topLosersModel, cropped);

    ui->top_gainers_list->setModel(topGainersModel);
    ui->top_losers_list->setModel(topLosersModel);
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
    ui->listWidget->setStyleSheet("QListWidget { border-radius: 10px; background-color: rgb(222, 222, 222); }");
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
    MainWindow::openShares(figi.toStdString(), stockName.toStdString(), token);
}

void MainWindow::on_topGainersList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    if (selectedItem != "No data for selected period")
        QMessageBox::information(this, "Top Gainer Selected", "You selected: " + selectedItem);
    else 
        QMessageBox::information(this, "Error", "Either today is Sunday or there is an error on the server");
}


void MainWindow::on_topLosersList_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    QMessageBox::information(this, "Top Loser Selected", "You selected: " + selectedItem);
}


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedItem = index.data().toString();
    ui->lineEdit->setText(selectedItem);
    std::string figi = selectedItem.split("\t")[1].toStdString(); 
    std::string stockName = selectedItem.section('\t', 0, 0).toStdString();   
    QString token;
    MainWindow::openShares(figi, stockName, token);
}


void MainWindow::openShares(const std::string& figi, const std::string& stockName, QString token)
{
    shares *window1 = new shares(this, figi, stockName, token);
    window1->show();
}
