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

void insertDataIntoDatabase(const std::string& open_units, const std::string& open_nano, 
                            const std::string& high_units, const std::string& high_nano,
                            const std::string& low_units, const std::string& low_nano, 
                            const std::string& close_units, const std::string& close_nano, 
                            const std::string& volume, const std::string& time_seconds) {
    sqlite3* db;
    char* errMsg = 0;

    int rc = sqlite3_open("candles.db", &db);

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
                               "time_seconds INTEGER);";

    rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);

    sqlStatement = "INSERT INTO candles (open_units, open_nano, high_units, high_nano, "
                   "low_units, low_nano, close_units, close_nano, volume, time_seconds) VALUES (" + 
                   open_units + ", " + open_nano + ", " + high_units + ", " + high_nano + ", " + 
                   low_units + ", " + low_nano + ", " + close_units + ", " + close_nano + ", " + 
                   volume + ", " + time_seconds + ");";

    rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);

    sqlite3_close(db);
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

    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox-

    auto sandbox = std::dynamic_pointer_cast<Sandbox>(client.service("sandbox"));
    sandbox->OpenSandboxAccount();
    auto accounts = sandbox->GetSandboxAccounts();

    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    auto candlesServiceReply = marketdata->GetCandles("BBG004730JJ5", 1716184798, 0, 1716368398, 0, CandleInterval::CANDLE_INTERVAL_HOUR);
        
    auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());

    for (int i = 0; i < response->candles_size(); i++) {
        auto candle = response->candles(i);
        std::string open_units = std::to_string(candle.open().units());
        std::string open_nano = std::to_string(candle.open().nano());
        std::string high_units = std::to_string(candle.high().units());
        std::string high_nano = std::to_string(candle.high().nano());
        std::string low_units = std::to_string(candle.low().units());
        std::string low_nano = std::to_string(candle.low().nano());
        std::string close_units = std::to_string(candle.close().units());
        std::string close_nano = std::to_string(candle.close().nano());
        std::string volume = std::to_string(candle.volume());
        std::string time_seconds = std::to_string(candle.time().seconds());

        insertDataIntoDatabase(open_units, open_nano, high_units, high_nano, low_units, low_nano, close_units, close_nano, volume, time_seconds);
    }


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