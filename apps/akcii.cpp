#include "akcii.h"
#include "ui_akcii.h"
#include <QtCharts>
#include <QtSql>

#include <QMessageBox>
#include <QFile>

#include <QDateTime>
#include <QVBoxLayout>
#include <QTimer>

#include "investapiclient.h"
#include "sandboxservice.h"
#include "marketdataservice.h"

#include <sqlite3.h>

void showError(const QString &message) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(message);
    msgBox.exec();
}

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

akcii::akcii(QWidget *parent, const std::string& figi, const std::string& stockName)
    : QDialog(parent)
    , ui(new Ui::akcii)
    , figi(figi)
    , stockName(stockName) 
{
    ui->setupUi(this);
    this->resize(840, 520);
    this->setMinimumSize(this->size());
    ui->stocknameLabel->setText(QString::fromStdString(stockName));
    
    ui->intervalComboBox->addItem("1 minute");
    ui->intervalComboBox->addItem("2 minutes");
    ui->intervalComboBox->addItem("3 minutes");
    ui->intervalComboBox->addItem("5 minutes");
    ui->intervalComboBox->addItem("10 minutes");
    ui->intervalComboBox->addItem("15 minutes");
    ui->intervalComboBox->addItem("30 minutes");

    ui->intervalComboBox->addItem("1 hour");
    ui->intervalComboBox->addItem("2 hours");
    ui->intervalComboBox->addItem("4 hours");

    ui->intervalComboBox->addItem("1 day");
    ui->intervalComboBox->addItem("1 week");
    ui->intervalComboBox->addItem("1 month");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &akcii::updateDateTime);
    timer->start(1000);

    // connect(ui->datefromInput, &QDateTimeEdit::dateTimeChanged, this, &akcii::fetchCandlestickData);
    // connect(ui->datetoInput, &QDateTimeEdit::dateTimeChanged, this, &akcii::fetchCandlestickData);
    // connect(ui->intervalComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &akcii::fetchCandlestickData);

    connect(ui->updateButton, &QPushButton::clicked, this, &akcii::fetchCandlestickData);
}

akcii::~akcii() {delete ui;}

void akcii::fetchCandlestickData() {
    QDateTime fromDateTime = ui->datefromInput->dateTime();
    QDateTime toDateTime = ui->datetoInput->dateTime();

    qint64 fromEpoch = fromDateTime.toSecsSinceEpoch();
    qint64 toEpoch = toDateTime.toSecsSinceEpoch();

    clearDatabase();

    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox

    QString intervalText = ui->intervalComboBox->currentText();
    CandleInterval interval = CandleInterval::CANDLE_INTERVAL_UNSPECIFIED;
    if (intervalText == "1 minute") {
        interval = CandleInterval::CANDLE_INTERVAL_1_MIN;
    } else if (intervalText == "2 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_2_MIN;
    } else if (intervalText == "3 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_3_MIN;
    } else if (intervalText == "10 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_10_MIN;
    } else if (intervalText == "15 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_15_MIN;
    } else if (intervalText == "30 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_30_MIN;
    } else if (intervalText == "1 hour") {
        interval = CandleInterval::CANDLE_INTERVAL_HOUR;
    } else if (intervalText == "2 hours") {
        interval = CandleInterval::CANDLE_INTERVAL_2_HOUR;
    } else if (intervalText == "4 hours") {
        interval = CandleInterval::CANDLE_INTERVAL_4_HOUR;
    } else if (intervalText == "1 day") {
        interval = CandleInterval::CANDLE_INTERVAL_DAY;
    } else if (intervalText == "1 week") {
        interval = CandleInterval::CANDLE_INTERVAL_WEEK;
    } else if (intervalText == "1 month") {
        interval = CandleInterval::CANDLE_INTERVAL_MONTH;
    }

    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    auto candlesServiceReply = marketdata->GetCandles(figi, fromEpoch, 0, toEpoch, 0, interval);
    auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());

    if (!response) {
        showError("Failed to retrieve candle data.");
        return;
    }

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

    updateChart();
}

void akcii::updateChart() {
    QCandlestickSeries *series = new QCandlestickSeries();
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    
    QSqlDatabase dbq = QSqlDatabase::addDatabase("QSQLITE");
    dbq.setDatabaseName("candles.db");

    QSqlQuery query;
    query.prepare("SELECT open_units, open_nano, high_units, high_nano, low_units, low_nano, close_units, close_nano, volume, time_seconds FROM candles");

    if (!query.exec()) {
        showError("Unable to make the correct query. Check your Internet connection.");
        return;
    }

    while (query.next()) {
        qreal open = query.value("open_units").toDouble() + query.value("open_nano").toDouble() / 1e9;
        qreal high = query.value("high_units").toDouble() + query.value("high_nano").toDouble() / 1e9;
        qreal low = query.value("low_units").toDouble() + query.value("low_nano").toDouble() / 1e9;
        qreal close = query.value("close_units").toDouble() + query.value("close_nano").toDouble() / 1e9;
        qint64 timestamp = query.value("time_seconds").toLongLong();
        QDateTime time = QDateTime::fromSecsSinceEpoch(timestamp);
        series->append(new QCandlestickSet(open, high, low, close, time.toMSecsSinceEpoch()));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM.yyyy hh:mm:ss");
    axisX->setTitleText("Time");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    chart->setTitle("Candlestick Chart");

    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->graphicsView->setLayout(layout);
}

void akcii::clearDatabase() {
    sqlite3* db;
    int rc = sqlite3_open("candles.db", &db);

    if (rc == SQLITE_OK) {
        std::string sqlStatement = "DELETE FROM candles";
        char* errMsg = 0;
        rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            showError("Internal error. Try later.");
            sqlite3_free(errMsg);
        }
        sqlite3_close(db);
    } else {
        showError("Can't open database");
    }
}

void akcii::clearChart() {
    QLayout *layout = ui->graphicsView->layout();
    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
}

void akcii::updateDateTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->dateLabel->setText(currentDateTime.toString("dddd, MMMM d"));
    ui->timeLabel->setText(currentDateTime.toString("h:mm:ss"));
}
