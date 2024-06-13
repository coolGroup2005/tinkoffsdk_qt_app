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
#include <QMouseEvent>
#include <QDebug> 
#include <QtConcurrent>
#include <QFile>

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
    tableModel(new QStandardItemModel(this)),
    textEdit(new SearchTextEdit(this))
{   
    initializeUI();
}

QStandardItem* DatabaseFigi::createNonEditableItem(const QString& text) {
    QStandardItem* item = new QStandardItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

void DatabaseFigi::initializeUI() {
    mainLayout = new QVBoxLayout(this);

    enterLabel = new QLabel("Enter company name", this);
    enterLabel->setStyleSheet("font-weight: bold; font-size: 20px;");
    mainLayout->addWidget(enterLabel);

    searchLayout = new QHBoxLayout();
    textEdit->setObjectName("input_figi"); 
    textEdit->setMaximumSize(QSize(16777215, 40));
    textEdit->setStyleSheet("QTextEdit#input_figi { border-radius: 10px; background-color: rgb(222, 222,  222); }"); 
    searchButton = new QPushButton("Search");
    ClickCounter::installOn(searchButton);

    searchButton->setObjectName("search_figi_btn"); 
    searchButton->setMinimumSize(QSize(0, 40));
    searchButton->setMaximumSize(QSize(16777215, 40));
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

    tableView->setStyleSheet("QTableView#table_figi { border-radius: 10px; background-color: rgb(222, 222, 222); }"
                         "QTableView#table_figi::first-row { background-color: rgb(193, 193, 193); }"
                         "QTableView#table_figi::first-column { background-color: rgb(193, 193, 193); }");
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ClickCounter::installOn(tableView);

    tableView->setModel(tableModel);
    mainLayout->addWidget(tableView);

    this->setStyleSheet(R"(
        QWidget {
            background-color: #ebeae7;
            color: #525252;
        }
        QLabel {
            color: #525252;
        }
        QLineEdit, QComboBox {
            background-color: #d3d2d0;
            border-radius: 15px;
            color: #525252;
            padding: 10px;
        }
        QHeaderView::section {
            background-color: lightgrey;
        }
        QComboBox::down-arrow {
            image: none;
        }
        QTableView {
            gridline-color: #b0b0af;
            background-color: #d3d2d0;
            border: 1px solid #d3d2d0;
            border-radius: 20px;
        }
        QTableView::item {
            background-color: #d3d2d0;
        }
        QTableView::item:selected {
            background-color: #b5cbd8;
            color: #000000;
        }
        QPushButton {
            background-color: rgb(193, 193, 193);
            border-radius: 20px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: rgb(170, 170, 170);
        }
        QPushButton:pressed {
            background-color: rgb(150, 150, 150);
        }
    )");

    setLayout(mainLayout);
}

void DatabaseFigi::initializeDatabase() {
    QString dbPath = "figi.db";
    // if (QFile::exists(dbPath)) {
    //     QFile::remove(dbPath);
    // }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        qDebug() << "Error: connection with database figi failed";
    } else {
        qDebug() << "Database figi: connection ok";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS figi (name TEXT COLLATE NOCASE, figi TEXT, trading_status TEXT)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_name ON figi (name)");
}

void DatabaseFigi::setupConnections() {
    connect(searchButton, &QPushButton::clicked, this, &DatabaseFigi::onSearchButtonClicked);
    connect(textEdit, &SearchTextEdit::enterPressed, this, &DatabaseFigi::onSearchButtonClicked);
    //connect(tableView, &QTableView::clicked, this, &DatabaseFigi::onTableDoubleClicked);
}

void DatabaseFigi::onTableDoubleClicked(const QModelIndex &index) {
    QString name = tableModel->item(index.row(), 0)->text();
    QString figi = tableModel->item(index.row(), 1)->text();
    std::string _token = "t.oKorE_EYwJfNIVocBju-zSeLPhq4SbM4LgSVF0gJUShLQBk7z16xHYxZd489LUPnD3djym7T1SKSavBLwhs3hA";
    std::cout << _token << name.toStdString();
    mainWindow->openShares(figi.toStdString(), name.toStdString(), QString::fromStdString(_token));

}

void DatabaseFigi::insertSharesIntoDatabase(QString token) {
    initializeDatabase();
    setupConnections();
    loadAllShares(); 
    InvestApiClient client("invest-public-api.tinkoff.ru:443", token.toStdString());
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto answerShares = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply = dynamic_cast<SharesResponse*>(answerShares.ptr().get());

    QSqlQuery query;
    query.prepare("INSERT INTO figi (name, figi, trading_status) VALUES (:name, :figi, :trading_status)");

    db.transaction();
    for (int i = 0; i < answerShareReply->instruments_size(); i++) {
        query.bindValue(":name", QString::fromStdString(answerShareReply->instruments(i).name()));
        query.bindValue(":figi", QString::fromStdString(answerShareReply->instruments(i).figi()));
        query.bindValue(":trading_status", QString::fromStdString(formatTradingStatus(answerShareReply->instruments(i).trading_status())));
        if (!query.exec()) {
            qDebug() << "Error inserting into database figi: " << query.lastError();
        }
    }
    db.commit();
}

void DatabaseFigi::onSearchButtonClicked() {
    QString searchText = textEdit->toPlainText();

    QSqlQuery query;
    query.prepare("SELECT name, figi, trading_status FROM figi WHERE name LIKE :name COLLATE NOCASE");
    query.bindValue(":name", "%" + searchText + "%");

    if (!query.exec()) {
        qDebug() << "Error searching in database figi: " << query.lastError();
        return;
    }

    QList<QList<QStandardItem*>> items;
    QSet<QString> uniqueEntries; 

    while (query.next()) {
        QString name = query.value("name").toString();
        QString figi = query.value("figi").toString();
        QString trading_status = query.value("trading_status").toString();

        QString entryKey = name + figi + trading_status; 
        if (!uniqueEntries.contains(entryKey)) {
            uniqueEntries.insert(entryKey);

            QList<QStandardItem *> rowItems;
            rowItems.append(createNonEditableItem(name));
            rowItems.append(createNonEditableItem(figi));
            rowItems.append(createNonEditableItem(trading_status));
            items.append(rowItems);
        }
    }

    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Name", "FIGI", "Trading Status"});
    for (auto &itemRow : items) {
        tableModel->appendRow(itemRow);
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
            items.append(createNonEditableItem(name));
            items.append(createNonEditableItem(figi));
            items.append(createNonEditableItem(trading_status));
            tableModel->appendRow(items);
        }
    }
    int numElements = tableModel->rowCount();
    numElementsLabel->setText(QString::number(numElements) + " elements found");
}

size_t ClickCounter::clickCount = 0;

ClickCounter::ClickCounter(QObject *parent)
    : QObject(parent) {}

void ClickCounter::installOn(QWidget *widget) {
    widget->installEventFilter(new ClickCounter(widget));
}

bool ClickCounter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        incrementClickCount();
    }
    return QObject::eventFilter(obj, event);
}

void ClickCounter::incrementClickCount() {
    ++clickCount;
    std::cout << "Click count:" << clickCount << std::endl;
}
