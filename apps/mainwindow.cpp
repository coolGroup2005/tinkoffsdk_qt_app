#include "mainwindow.h"
#include "akcii.h"
#include "ui_mainwindow.h"
#include "homepage/homepage.h"


#include <QStringList>
#include <QStringListModel>
#include <QMessageBox>
#include <QStandardItemModel>

#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Database Fiji");
    ui->tabWidget->setTabText(1, "Portfolio");
    ui->tabWidget->setTabText(2, "Statistics");
    ui->tabWidget->setTabText(3, "Home");

    // Iteraction with tab Home
    std::vector<ShareInfo> sharesList;
    sharesList = parseFigi();

    const int numRows = sharesList.size();
    const int numColumns = 3;

    QStandardItemModel* model = new QStandardItemModel(numRows, numColumns);
    model->setHeaderData(0, Qt::Horizontal, "Name", Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, "Figi", Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, "Trading Status", Qt::DisplayRole);
    for (int i = 0; i < numRows; ++i)
    {
        QList<QStandardItem*> itemsList;
        itemsList.append(new QStandardItem(QString::fromStdString(sharesList[i].name)));
        itemsList.append(new QStandardItem(QString::fromStdString(sharesList[i].figi)));
        itemsList.append(new QStandardItem(QString::fromStdString(formatTradingStatus(sharesList[i].trading_status))));
        for (short j = 0; j < 3; ++j)
        {
            itemsList[j]->setEditable(false);
            model->setItem(i, j, itemsList[j]);
        }
    }

    // const int numRows = 10;
    // const int numColumns = 10;

    // QStandardItemModel* model = new QStandardItemModel(numRows, numColumns);
    // for (int row = 0; row < numRows; ++row)
    // {
    //     for (int column = 0; column < numColumns; ++column)
    //     {
    //         QString text = QString::fromStdString('A' + std::to_string(row)) + QString::number(column + 1);
    //         QStandardItem* item = new QStandardItem(text);
    //         model->setItem(row, column, item);
    //     }
    //  }

    ui->sharesTableView->setModel(model);
    ui->sharesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->sharesTableView->verticalHeader()->setVisible(false);
    ui->sharesTableView->setStyleSheet("QHeaderView::section {background-color: lightgrey}");

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
    QString selectedName = ui->sharesTableView->model()->index(index.row(),0).data().toString();
    ui->lineEdit->setText(selectedName);
    MainWindow::openAkcii();
}

void MainWindow::openAkcii()
{
    akcii *window1 = new akcii(this);
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


