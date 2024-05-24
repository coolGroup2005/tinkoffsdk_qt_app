#include "akcii.h"
#include "ui_akcii.h"
#include <QtCharts>
#include <QtSql>

#include "investapiclient.h"
#include "sandboxservice.h"
#include "marketdataservice.h"

#include <sstream>
#include <vector>
#include <sqlite3.h>

struct Time {
    long seconds;
};

struct Price {
    int units;
    int nano;
};

struct Candle_STRUCT {
    Price open;
    Price high;
    Price low;
    Price close;
    int volume;
    Time time;
    bool is_complete;
    int unknown_field_9;
};

std::vector<Candle_STRUCT> parseCandles(const std::string& input) {
    std::vector<Candle_STRUCT> candles;
    std::istringstream stream(input);
    std::string line;
    Candle_STRUCT currentCandle;
    bool insideCandle = false;

    while (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        std::string key;
        lineStream >> key;

        if (key == "candles") {
            if (insideCandle) {
                candles.push_back(currentCandle);
                currentCandle = Candle_STRUCT();
            }
            insideCandle = true;
        } else if (key == "open" or key == "high" or key == "low" or key == "close") {
            Price price;
            std::string S, N;
            std::getline(stream, S, ':');
            std::getline(stream, S);
            std::getline(stream, N, ':');
            std::getline(stream, N);
            price.units = std::stoi(S);
            price.nano = std::stoi(N);

            if (key == "open") currentCandle.open = price;
            if (key == "high") currentCandle.high = price;
            if (key == "low") currentCandle.low = price;
            if (key == "close") currentCandle.close = price;

        } else if (key == "volume") {
            std::string volumeStr;
            std::getline(stream, volumeStr, ':');
            std::getline(stream, volumeStr);
            currentCandle.volume = std::stoi(volumeStr);

        } else if (key == "time") {
            std::string secondsStr;
            std::getline(stream, secondsStr, ':');
            std::getline(stream, secondsStr);
            currentCandle.time.seconds = std::stol(secondsStr);

        } else if (key == "is_complete") {
            std::string isCompleteStr;
            std::getline(stream, isCompleteStr, ':');
            std::getline(stream, isCompleteStr);
            currentCandle.is_complete = (isCompleteStr == "true");

        }
    }
    if (insideCandle) candles.push_back(currentCandle);
    return candles;
}

// void printCandle(const Candle_STRUCT& candle) {
//     std::cout << "Candle:" << std::endl;
//     std::cout << "Open: " << candle.open.units << "." << candle.open.nano << std::endl;
//     std::cout << "High: " << candle.high.units << "." << candle.high.nano << std::endl;
//     std::cout << "Low: " << candle.low.units << "." << candle.low.nano << std::endl;
//     std::cout << "Close: " << candle.close.units << "." << candle.close.nano << std::endl;
//     std::cout << "Volume: " << candle.volume << std::endl;
//     std::cout << "Time_seconds): " << candle.time.seconds << std::endl;
//     std::cout << "Is Complete: " << (candle.is_complete ? "true": "false") << std::endl;
// }

void createCandlesTable(sqlite3* db) {
    char* errorMessage;
    std::string sqlStatement = "CREATE TABLE IF NOT EXISTS candles ("
                               "open_units INTEGER, "
                               "open_nano INTEGER, "
                               "high_units INTEGER, "
                               "high_nano INTEGER, "
                               "low_units INTEGER, "
                               "low_nano INTEGER, "
                               "close_units INTEGER, "
                               "close_nano INTEGER, "
                               "volume INTEGER, "
                               "time_seconds INTEGER, "
                               "is_complete INTEGER)";

    int result = sqlite3_exec(db, sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
}

void insertCandleData(sqlite3* db, const Candle_STRUCT& candle) {
    std::stringstream sqlStatement;
    sqlStatement << "INSERT INTO candles VALUES ("
                 << candle.open.units << ", "
                 << candle.open.nano << ", "
                 << candle.high.units << ", "
                 << candle.high.nano << ", "
                 << candle.low.units << ", "
                 << candle.low.nano << ", "
                 << candle.close.units << ", "
                 << candle.close.nano << ", "
                 << candle.volume << ", "
                 << candle.time.seconds << ", "
                 << (candle.is_complete ? 1 : 0) << ")";

    char* errorMessage;
    int result = sqlite3_exec(db, sqlStatement.str().c_str(), nullptr, nullptr, &errorMessage);
}

akcii::akcii(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::akcii)
{
    ui->setupUi(this);
    this->resize(840, 520);
    this->setFixedSize(this->size()); //lock an ability to change the size

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &akcii::updateDateTime);
    timer->start(1000);

    sqlite3* db;
    int rc = sqlite3_open("candles.db", &db);

    createCandlesTable(db);

    InvestApiClient client("invest-public-api.tinkoff.ru:443", "TOKEN");
    auto sandbox = std::dynamic_pointer_cast<Sandbox>(client.service("sandbox"));
    sandbox->OpenSandboxAccount();
    auto accounts = sandbox->GetSandboxAccounts();

    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    auto candlesServiceReply = marketdata->GetCandles("BBG004730JJ5", 1716184798, 0, 1716368398, 0, CandleInterval::CANDLE_INTERVAL_HOUR);
    
    auto unparsedCandles = candlesServiceReply.ptr()->DebugString();

    std::vector<Candle_STRUCT> candles = parseCandles(unparsedCandles);
    for (const auto& candle : candles) insertCandleData(db, candle);

    sqlite3_close(db);

    QCandlestickSeries *series = new QCandlestickSeries();
    
    QSqlDatabase dbq = QSqlDatabase::addDatabase("QSQLITE");
    dbq.setDatabaseName("candles.db");

    if (!dbq.open()) {
        qDebug() << "Unable to open the database:" << dbq.lastError().text();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT open_units, open_nano, high_units, high_nano, low_units, close_units, close_nano, volume, time_seconds FROM candles");

    if (!query.exec()) {
        qDebug() << "Unable to make the correct query:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        qreal open = query.value("open_units").toDouble() + query.value("open_nano").toDouble() / 1e9;
        qreal high = query.value("high_units").toDouble() + query.value("high_nano").toDouble() / 1e9;
        qreal low = query.value("low_units").toDouble();
        qreal close = query.value("close_units").toDouble() + query.value("close_nano").toDouble() / 1e9;
        qint64 timestamp = query.value("time_seconds").toLongLong();
        series->append(new QCandlestickSet(open, high, low, close, timestamp));
    }

    QChart *chart = new QChart();
    QChartView *chartView = new QChartView(chart);

    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTickCount(10);
    axisX->setFormat("MM-dd hh:mm");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->graphicsView->setLayout(layout);
}

akcii::~akcii()
{
    delete ui;
}

void akcii::updateDateTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->dateLabel->setText(currentDateTime.toString("dddd, MMMM d"));
    ui->timeLabel->setText(currentDateTime.toString("h:mm:ss"));
}