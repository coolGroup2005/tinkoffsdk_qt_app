#include <iostream>
#include <vector>
#include <QObject>
#include <QStringListModel>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QSqlError>
#include <QSqlRecord>
#include <QHeaderView> 
#include <QStandardItemModel>
#include <QSet> 

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"
#include "figi.h"
#include "./homepage/homepage.h"

DatabaseFigi::DatabaseFigi(QWidget *parent)
    : QWidget(parent),
    tableView(new QTableView(this)),
    tableModel(new QStandardItemModel(this))
{
    initializeUI();
    initializeDatabase();
    setupConnections();
    loadAllShares(); 
}

void DatabaseFigi::initializeUI() {
    mainLayout = new QVBoxLayout(this);

    enterLabel = new QLabel("Enter company name", this);
    enterLabel->setStyleSheet("font-weight: bold; font-size: 20px;");
    mainLayout->addWidget(enterLabel);

    searchLayout = new QHBoxLayout();
    textEdit = new QTextEdit();
    textEdit->setObjectName("input_figi"); 
    textEdit->setMaximumSize(QSize(16777215, 40));
    textEdit->setStyleSheet("QTextEdit#input_figi { border-radius: 10px; background-color: rgb(222, 222,  222); }"); 
    searchButton = new QPushButton("Search");
    searchButton->setObjectName("search_figi_btn"); 
    searchButton->setMinimumSize(QSize(0, 40));
    searchButton->setMaximumSize(QSize(16777215, 40));
    searchButton->setStyleSheet("QPushButton#search_figi_btn { margin: 0; background-color: rgb(193, 193, 193); padding: 5px; border-radius: 8px; }"); 
    searchLayout->addWidget(textEdit);
    searchLayout->addWidget(searchButton);
    mainLayout->addLayout(searchLayout);

    suggestionsLabel = new QLabel("Suggestions:", this);
    suggestionsLabel->setStyleSheet("font-weight: bold; font-size: 20px; "); 
    numElementsLabel = new QLabel("", this);
    numElementsLabel->setStyleSheet("font-size: 10px;"); 
    mainLayout->addWidget(suggestionsLabel);
    mainLayout->addWidget(numElementsLabel);
    tableView->setObjectName("table_figi"); 

    tableView->setStyleSheet("QTableView#table_figi { border-radius: 10px; background-color: rgb(222, 222, 222); gridline-color: rgb(100, 100, 100) }"
                         "QTableView#table_figi::first-row { background-color: rgb(193, 193, 193); }"
                         "QTableView#table_figi::first-column { background-color: rgb(193, 193, 193); }");
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setModel(tableModel);
    mainLayout->addWidget(tableView);

    setLayout(mainLayout);
}

void DatabaseFigi::initializeDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("figi.db");
    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
    } else {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS figi (name TEXT COLLATE NOCASE, figi TEXT, trading_status TEXT)");
}

void DatabaseFigi::setupConnections() {
    connect(searchButton, &QPushButton::clicked, this, &DatabaseFigi::onSearchButtonClicked);
}

void DatabaseFigi::insertSharesIntoDatabase() {
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto answerShares = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply = dynamic_cast<SharesResponse*>(answerShares.ptr().get());

    QSqlQuery query;
    query.prepare("INSERT INTO figi (name, figi, trading_status) VALUES (:name, :figi, :trading_status)");

    for (int i = 0; i < answerShareReply->instruments_size(); i++) {
        std::cout << answerShareReply->instruments(i).name() << std::endl;
        query.bindValue(":name", QString::fromStdString(answerShareReply->instruments(i).name()));
        query.bindValue(":figi", QString::fromStdString(answerShareReply->instruments(i).figi()));
        query.bindValue(":trading_status", QString::fromStdString(formatTradingStatus(answerShareReply->instruments(i).trading_status())));
        if (!query.exec()) {
            qDebug() << "Error inserting into table: " << query.lastError();
        }
    }
}

void DatabaseFigi::onSearchButtonClicked() {
    QString searchText = textEdit->toPlainText();
    QSqlQuery query;
    query.prepare("SELECT name, figi, trading_status FROM figi WHERE name LIKE :name COLLATE NOCASE");
    query.bindValue(":name", "%" + searchText + "%");

    if (!query.exec()) {
        qDebug() << "Error searching table: " << query.lastError();
        return;
    }

    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Name", "FIGI", "Trading Status"});

    QSet<QString> uniqueEntries; 

    while (query.next()) {
        QString name = query.value("name").toString();
        QString figi = query.value("figi").toString();
        QString trading_status = query.value("trading_status").toString();

        QString entryKey = name + figi + trading_status; 
        if (!uniqueEntries.contains(entryKey)) {
            uniqueEntries.insert(entryKey);

            QList<QStandardItem *> items;
            items.append(new QStandardItem(name));
            items.append(new QStandardItem(figi));
            items.append(new QStandardItem(trading_status));
            tableModel->appendRow(items);
        }
    }

    int numElements = tableModel->rowCount();
    numElementsLabel->setText(QString::number(numElements) + " elements found");
}

void DatabaseFigi::loadAllShares() {
    QSqlQuery query("SELECT name, figi, trading_status FROM figi");

    if (!query.exec()) {
        qDebug() << "Error loading all shares: " << query.lastError();
        return;
    }

    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Name", "FIGI", "Trading Status"});

    QSet<QString> uniqueEntries; 

    while (query.next()) {
        QString name = query.value("name").toString();
        QString figi = query.value("figi").toString();
        QString trading_status = query.value("trading_status").toString();

        QString entryKey = name + figi + trading_status; 
        if (!uniqueEntries.contains(entryKey)) {
            uniqueEntries.insert(entryKey);

            QList<QStandardItem *> items;
            items.append(new QStandardItem(name));
            items.append(new QStandardItem(figi));
            items.append(new QStandardItem(trading_status));
            tableModel->appendRow(items);
        }
    }
    int numElements = tableModel->rowCount();
    numElementsLabel->setText(QString::number(numElements) + " elements found");
}
