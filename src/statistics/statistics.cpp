#include "statistics/statistics.h"
#include <QDebug> 
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <cmath>
#include <utility>
#include <string>
#include <sqlite3.h>
#include <filesystem>
#include <algorithm>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>


const int MOEX_START_HOUR = 9;
const int MOEX_START_MINUTE = 30;
const int MOEX_END_HOUR = 23;
const int MOEX_END_MINUTE = 50;

bool isWeekend(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    int wday = tm->tm_wday;
    return (wday == 0 || wday == 6); // 0 = Sunday, 6 = Saturday
}

bool isOutsideWorkingHours(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    int hour = tm->tm_hour;
    int minute = tm->tm_min;

    if (hour < MOEX_START_HOUR || (hour == MOEX_START_HOUR && minute < MOEX_START_MINUTE)) {
        return true;
    }
    if (hour > MOEX_END_HOUR || (hour == MOEX_END_HOUR && minute > MOEX_END_MINUTE)) {
        return true;
    }
    return false;
}

std::time_t adjustToWorkingTime(std::time_t time) {
    std::tm* tm = std::localtime(&time);

    if (isWeekend(time)) {
        // If it's Saturday, move to the next Monday
        // If it's Sunday, move to the next Monday
        int daysToAdd = (tm->tm_wday == 6) ? 2 : 1;
        tm->tm_mday += daysToAdd;
    } else if (isOutsideWorkingHours(time)) {
        if (tm->tm_hour < MOEX_START_HOUR || (tm->tm_hour == MOEX_START_HOUR && tm->tm_min < MOEX_START_MINUTE)) {
            tm->tm_hour = MOEX_START_HOUR;
            tm->tm_min = MOEX_START_MINUTE;
        } else if (tm->tm_hour > MOEX_END_HOUR || (tm->tm_hour == MOEX_END_HOUR && tm->tm_min > MOEX_END_MINUTE)) {
            tm->tm_mday += 1;
            tm->tm_hour = MOEX_START_HOUR;
            tm->tm_min = MOEX_START_MINUTE;
        }
    }

    return std::mktime(tm);
}


void clearDatabaseStatistics() {
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("statistics.db", &db);

    if (rc == SQLITE_OK) {
        std::string sqlStatement = "DELETE FROM statistics";
        rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            std::cout << "Database cleared" << std::endl;
        }
        sqlite3_close(db);
    } else {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }
}




float getShareChange(std::string& figi, std::time_t& dateFromToTime, std::time_t& currentTime)  {
    
    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN"));
    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    if (!marketdata) {
        qDebug() << "error marketdata";
        return 10000;
    }

    auto candlesServiceReply = marketdata->GetCandles(figi, dateFromToTime, 0, currentTime, 0, CandleInterval::CANDLE_INTERVAL_DAY);
    auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());
    if (!response) {
        qDebug() << "error response";
        return 10000;
    }

    if (response->candles_size() == 0) {
        qDebug() << "No candles found in the response";
        return 10000; 
    }

    auto firstCandle = response->candles(0);
    auto lastCandle = response->candles(response->candles_size() - 1);

    auto open_units = firstCandle.open().units();
    auto open_nano = firstCandle.open().nano();
    auto close_units = lastCandle.close().units();
    auto close_nano = lastCandle.close().nano();

    if (!open_units && !open_nano) {
        qDebug() << "Invalid open units or nano";
        return 10000;
    }

    if (!close_units && !close_nano) {
        qDebug() << "Invalid close units or nano";
        return 10000;
    }

    float total_open = open_units + open_nano / (std::pow(10, std::to_string(open_nano).length()));
    float total_close = close_units + close_nano / (std::pow(10, std::to_string(close_nano).length()));

    float x = (total_close - total_open) * 100 / total_open;
    return x;
}

SharesVector getAllSharesWithChange(InvestApiClient& client, int& interval, bool cropped)
{
    SharesVector allShares;
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));

    auto answerShares = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply = dynamic_cast<SharesResponse*>(answerShares.ptr().get());

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::time_t dateFromToTime;

    switch (interval) {
        case 0: {
            auto dateFrom = now - std::chrono::hours(24);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        case 1: {
            auto dateFrom = now - std::chrono::hours(24 * 7);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        case 2: {
            auto dateFrom = now - std::chrono::hours(24 * 30);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        default: {
            std::cerr << "Invalid interval type" << std::endl;
            break;
        }
    }

    dateFromToTime = adjustToWorkingTime(dateFromToTime);
    currentTime = adjustToWorkingTime(currentTime);
    int size = cropped ? (answerShareReply->instruments_size() - 1)/9 : answerShareReply->instruments_size() - 1;

    for (int i = 0; i < size; i++) {
        unsigned int tradingStatus = answerShareReply->instruments(i).trading_status();
        std::string currency = answerShareReply->instruments(i).currency();
            // std::cout << i << "trading status: " << tradingStatus << std::endl;
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        bool now_time_not_ok = isWeekend(now_time) || isOutsideWorkingHours(now_time);

        if (tradingStatus == 5 || (now_time_not_ok && currency == "rub")) {
            std::string name =  answerShareReply->instruments(i).name();
            std::string figi =  answerShareReply->instruments(i).figi();
            MoneyValue nominal =  answerShareReply->instruments(i).nominal();
            ShareInfo share {name, figi, tradingStatus, currency, nominal};

            try {
                float shareChange = getShareChange(share.figi, dateFromToTime, currentTime);
                // std::cout << share.figi << '\t' << share.name << '\t' << shareChange << '\n';
                if (shareChange != 10000) {
                    auto pair = std::make_pair(share, shareChange);
                    allShares.push_back(pair);
                }

            } catch (...) {
                std::cerr << "error getAllSharesWithChange " << '\n';
                continue;
            }
        }
    }


    for (int i = 0; i < allShares.size(); ++i) {
        std::cout << allShares[i].first.name << '\t' << allShares[i].second << '\n';
    }
    clearDatabaseStatistics();
    return allShares;
    
}



void insertStatisticsIntoDatabase(SharesVector& sharesVector) {    
    std::sort(sharesVector.begin(), sharesVector.end(), 
            [](const std::pair<ShareInfo, float>& a, const std::pair<ShareInfo, float>& b) {
                // bool i = a.second < b.second;
                // std::cout << a.second << " " << b.second << i << std::endl;
                return a.second < b.second;
            });

    // for (const auto& sharePair : sharesVector) {
    //     std::cout << sharePair.first.figi << " " << sharePair.second << std::endl;
    // }

    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("statistics.db", &db);

    std::string sqlStatement = "CREATE TABLE IF NOT EXISTS statistics ("
                               "ID INTEGER PRIMARY KEY  AUTOINCREMENT, "
                               "company_name TEXT, "
                               "company_figi TEXT, "
                               "total_price_change REAL);";
    rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    std::cout << "Database created statistics" << std::endl;

    sqlite3_stmt* stmt;
    sqlStatement = "INSERT INTO statistics (company_name, company_figi, total_price_change) VALUES (?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sqlStatement.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    } else {
        for (const auto& sharePair : sharesVector) {
            const ShareInfo& share = sharePair.first;
            float priceChange = sharePair.second;

            sqlite3_bind_text(stmt, 1, share.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, share.figi.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 3, priceChange);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
                break;
            }
            sqlite3_reset(stmt); 
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Inserted all statistics window" << std::endl;
}

std::vector<std::pair<std::string, float>> getTopFromDb(std::string type) {
    std::vector<std::pair<std::string, float>> topShares;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("statistics.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return topShares;
    }

    std::string sql = "SELECT company_name, company_figi, total_price_change "
                    "FROM statistics "
                    "ORDER BY total_price_change " + type + " " +
                    "LIMIT 7";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);           
        return topShares;
    }


    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // ShareInfo share;
        // share.figi = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        float priceChange = static_cast<float>(sqlite3_column_double(stmt, 2));

        // std::cout << name << " " << priceChange << std::endl;
        auto pair = std::make_pair(name, priceChange);
        topShares.push_back(pair);
    }

    // for (int i = 0; i < topShares.size() - 1; i++) {
    //     std::cout << "Company: " << topShares[i].first << "\n";
    // }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return topShares;
}


StatisticsManager::StatisticsManager(QObject *parent) : QObject(parent)
{
}

void StatisticsManager::updateStatistics(int interval, QStringListModel* topGainersModel, QStringListModel* topLosersModel, QStringListModel* topActiveModel, bool cropped)
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));
    auto allShares = getAllSharesWithChange(client, interval, cropped);
    insertStatisticsIntoDatabase(allShares);
    std::vector<std::pair<std::string, float>> bottom = getTopFromDb("ASC");
    std::vector<std::pair<std::string, float>> top = getTopFromDb("DESC");

    QStringList topGainers;
    QStringList topLosers;

    for (const auto& sharePair : top) {
        // qDebug() << "Company: " << QString::fromStdString(sharePair.first) << "\n"
        //          << "Price Change: " << sharePair.second << "%\n"
        //          << "-----------------------------\n";
        QString gainer = QString::fromStdString(sharePair.first) + " (" + QString::number(sharePair.second) + ")";
        topGainers.append(gainer);
    }

     for (const auto& sharePair : bottom) {
        QString loser = QString::fromStdString(sharePair.first) + " (" + QString::number(sharePair.second) + ")";
        topLosers.append(loser);
    }

    topGainersModel->setStringList(topGainers);
    topLosersModel->setStringList(topLosers);

    emit statisticsUpdated();
}

DatabaseFigi::DatabaseFigi(QWidget *parent)
    : QWidget(parent),
      textEdit(new QTextEdit(this)),
      tableView(new QTableView(this)),
      tableModel(new QStandardItemModel(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Enter company name", this);
    QPushButton *searchButton = new QPushButton("Search", this);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(textEdit);
    searchLayout->addWidget(searchButton);

    mainLayout->addWidget(label);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(new QLabel("Suggestions:", this));
    mainLayout->addWidget(tableView);

    tableView->setModel(tableModel);
    connect(searchButton, &QPushButton::clicked, this, &DatabaseFigi::onSearchButtonClicked);

    loadAllShares();  // Load the entire table on initialization
}

void DatabaseFigi::initializeUI() {
    mainLayout = new QVBoxLayout(this);

    enterLabel = new QLabel("Enter company name");
    mainLayout->addWidget(enterLabel);

    searchLayout = new QHBoxLayout();
    textEdit = new QTextEdit();
    textEdit->setMaximumSize(QSize(16777215, 40));
    textEdit->setStyleSheet("QTextEdit#textEdit { border-radius: 10px; background-color: rgb(222, 222,  222); }");
    searchButton = new QPushButton("Search");
    searchButton->setMinimumSize(QSize(0, 40));
    searchButton->setMaximumSize(QSize(16777215, 40));
    searchButton->setStyleSheet("QPushButton#searchButton { margin: 0; background-color: rgb(193, 193, 193); padding: 5px; border-radius: 8px; }");
    searchLayout->addWidget(textEdit);
    searchLayout->addWidget(searchButton);
    mainLayout->addLayout(searchLayout);

    suggestionsLabel = new QLabel("Suggestions:");
    mainLayout->addWidget(suggestionsLabel);

    tableView = new QTableView();
    tableView->setStyleSheet("QTableView#tableView { border-radius: 10px; background-color: rgb(222, 222, 222); }");
    tableView->setModel(tableModel);
    mainLayout->addWidget(tableView);
}

void DatabaseFigi::initializeDatabase() {
    std::cout << " initializeDatabase--------------------------------" << std::endl;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("figi.db"); 
    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
    } else {
        qDebug() << "Database: connection ok";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE figi (name TEXT, figi TEXT, trading_status INTEGER)");
}

void DatabaseFigi::setupConnections() {
    std::cout << " setupConnections--------------------------------" << std::endl;

    connect(searchButton, &QPushButton::clicked, this, &DatabaseFigi::onSearchButtonClicked);
}

void DatabaseFigi::insertSharesIntoDatabase() {
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto answerShares = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply = dynamic_cast<SharesResponse*>(answerShares.ptr().get());

    QSqlQuery query;
    query.prepare("INSERT INTO figi (name, figi, trading_status) VALUES (:name, :figi, :trading_status)");
    std::cout << " before INSERT--------------------------------" << std::endl;

    for (int i = 0; i < answerShareReply->instruments_size() - 1; i++) {
        std::cout << answerShareReply->instruments(i).name() << std::endl;
            query.bindValue(":name", QString::fromStdString(answerShareReply->instruments(i).name()));
            query.bindValue(":figi", QString::fromStdString(answerShareReply->instruments(i).figi()));
            query.bindValue(":trading_status", answerShareReply->instruments(i).trading_status());
            if (!query.exec()) {
                qDebug() << "Error inserting into table: " << query.lastError();
            }
    }
}

void DatabaseFigi::onSearchButtonClicked() {
    QString searchText = textEdit->toPlainText();
    QSqlQuery query;
    query.prepare("SELECT name, figi, trading_status FROM shares WHERE name LIKE :name");
    query.bindValue(":name", "%" + searchText + "%");

    if (!query.exec()) {
        qDebug() << "Error searching table: " << query.lastError();
        return;
    }

    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Name", "FIGI", "Trading Status"});

    while (query.next()) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(query.value("name").toString()));
        items.append(new QStandardItem(query.value("figi").toString()));
        items.append(new QStandardItem(query.value("trading_status").toString()));
        tableModel->appendRow(items);
    }
}

void DatabaseFigi::loadAllShares() {
    QSqlQuery query("SELECT name, figi, trading_status FROM shares");

    if (!query.exec()) {
        qDebug() << "Error loading all shares: " << query.lastError();
        return;
    }

    tableModel->clear();
    tableModel->setHorizontalHeaderLabels({"Name", "FIGI", "Trading Status"});

    while (query.next()) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(query.value("name").toString()));
        items.append(new QStandardItem(query.value("figi").toString()));
        items.append(new QStandardItem(query.value("trading_status").toString()));
        tableModel->appendRow(items);
    }
}