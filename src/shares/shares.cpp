#include "shares/shares.h"
#include "ui_shares.h"


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
    if (rc != SQLITE_OK) {
        showError("SQL error: " + QString::fromStdString(errMsg));
        sqlite3_free(errMsg);
    }

    sqlStatement = "INSERT INTO candles (open_units, open_nano, high_units, high_nano, "
                   "low_units, low_nano, close_units, close_nano, volume, time_seconds) VALUES (" + 
                   open_units + ", " + open_nano + ", " + high_units + ", " + high_nano + ", " + 
                   low_units + ", " + low_nano + ", " + close_units + ", " + close_nano + ", " + 
                   volume + ", " + time_seconds + ");";
    rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
    sqlite3_close(db);
}

shares::shares(QWidget *parent, const std::string& figi, const std::string& stockName, const QString& token)
    : QDialog(parent)
    , ui(new Ui::shares)
    , figi(figi)
    , stockName(stockName)
    , token(token)
{
    ui->setupUi(this);
    this->resize(1040, 840);
    this->setMinimumSize(this->size());
    ui->stocknameLabel->setText(QString::fromStdString(stockName));

    ui->graphicsLineView->chart()->setBackgroundBrush(QBrush(QColor("#ebeae7")));
    ui->graphicsCandleView->chart()->setBackgroundBrush(QBrush(QColor("#ebeae7")));

    connect(ui->updateButton, &QPushButton::clicked, this, &shares::fetchCandlestickData);

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

    this->setStyleSheet(R"(
        QWidget {
            background-color: #ebeae7;
            color: #525252;
            border-color: #525252;
        }
        QLabel {
            color: #525252;
        }
        QLineEdit, QComboBox {
            background-color: #d3d2d0;
            border-radius: 15px;
            color: #525252;
            border-color: #525252;
        }
        QPushButton {
            background-color: rgb(193, 193, 193);
            border-radius: 15px;
        }
        QPushButton:hover {
            background-color: rgb(170, 170, 170);
        }
        QPushButton:pressed {
            background-color: rgb(150, 150, 150);
        }
    )");


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &shares::updateDateTime);
    timer->start(1000);

    updateDateTime();
}

shares::~shares() {
    delete ui;
}

void shares::fetchCandlestickData() {
    QDateTime fromDateTime = ui->datefromInput->dateTime();
    QDateTime toDateTime = ui->datetoInput->dateTime();
    qint64 fromEpoch = fromDateTime.toSecsSinceEpoch();
    qint64 toEpoch = toDateTime.toSecsSinceEpoch();

    clearDatabase();
    clearChart();

    InvestApiClient client("invest-public-api.tinkoff.ru:443", token.toStdString());

    QString intervalText = ui->intervalComboBox->currentText();
    CandleInterval interval = CandleInterval::CANDLE_INTERVAL_UNSPECIFIED;
    if (intervalText == "1 minute") {
        interval = CandleInterval::CANDLE_INTERVAL_1_MIN;
    } else if (intervalText == "2 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_2_MIN;
    } else if (intervalText == "3 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_3_MIN;
    } else if (intervalText == "5 minutes") {
        interval = CandleInterval::CANDLE_INTERVAL_5_MIN;
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

    for (size_t i = 0; i < response->candles_size(); i++) {
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

    updateCandleChart();
    updateLineChart();

}

void shares::updateCandleChart() {
    QCandlestickSeries *series = new QCandlestickSeries();
    series->setIncreasingColor(QColor(0, 207, 120, 254));
    series->setDecreasingColor(QColor(255, 104, 97, 254));
    
    connect(series, &QCandlestickSeries::hovered, this, &shares::showTooltipCandle);

    QSqlDatabase dbq = QSqlDatabase::addDatabase("QSQLITE");
    dbq.setDatabaseName("candles.db");
    if (!dbq.open()) {
        qDebug() << "Unable to open the database: " << dbq.lastError().text(); // без этой параши не работает
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT open_units, open_nano, high_units, high_nano, low_units, low_nano, close_units, close_nano, volume, time_seconds FROM candles");
    if (!query.exec()) {
        showError("Unable to make the correct query: " + query.lastError().text());
        return;
    }

    QVector<double> closePrices;
    QVector<qint64> timestamps;

    while (query.next()) {
        qreal open = query.value("open_units").toDouble() + query.value("open_nano").toDouble() / 1e9;
        qreal high = query.value("high_units").toDouble() + query.value("high_nano").toDouble() / 1e9;
        qreal low = query.value("low_units").toDouble() + query.value("low_nano").toDouble() / 1e9;
        qreal close = query.value("close_units").toDouble() + query.value("close_nano").toDouble() / 1e9;
        qint64 timestamp = query.value("time_seconds").toLongLong();
        QDateTime time = QDateTime::fromSecsSinceEpoch(timestamp);
        series->append(new QCandlestickSet(open, high, low, close, time.toMSecsSinceEpoch()));

        closePrices.append(close);
        timestamps.append(timestamp);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setBackgroundBrush(QBrush(QColor("#ebeae7")));  

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM.yyyy<br>hh:mm:ss");
    axisX->setTitleText("Time");
    axisX->setLinePenColor(QColor("#d3d2d0"));
    axisX->setGridLinePen(QColor("#d3d2d0"));
    //axisX->setLabelsAngle(-45);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price");
    axisY->setLinePenColor(QColor("#d3d2d0"));
    axisY->setGridLinePen(QColor("#d3d2d0"));
    //axisY->setLabelsAngle(-45);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLineSeries *smaSeries = new QLineSeries();
    int period = 9; // period like on TradeView
    for (int i = 0; i < closePrices.size(); ++i) {
        if (i >= period - 1) {
            double sum = 0.0;
            for (int j = 0; j < period; ++j) {
                sum += closePrices[i - j];
            }
            double sma = sum / period;
            smaSeries->append(timestamps[i] * 1000, sma);
        }
    }
    QPen smaPen(Qt::green);
    smaPen.setWidth(2);
    smaSeries->setPen(smaPen);

    QLineSeries *emaSeries = new QLineSeries();
    double multiplier = 2.0 / (period + 1);
    double ema = 0.0;
    
    for (int i = 0; i < closePrices.size(); ++i) {
        if (i == 0)
            ema = closePrices[i];
        else
            ema = ((closePrices[i] - ema) * multiplier) + ema;
        if (i >= period - 1)
            emaSeries->append(timestamps[i] * 1000, ema);
    }

    QPen emaPen(Qt::blue);
    emaPen.setWidth(2);
    emaSeries->setPen(emaPen);

    if (ui->smaCheckBox->isChecked())
        chart->addSeries(smaSeries);

    if (ui->emaCheckBox->isChecked())
        chart->addSeries(emaSeries);

    if (ui->smaCheckBox->isChecked()) {
        smaSeries->attachAxis(axisX);
        smaSeries->attachAxis(axisY);
    }

    if (ui->emaCheckBox->isChecked()) {
        emaSeries->attachAxis(axisX);
        emaSeries->attachAxis(axisY);

    }

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(chartView);
    ui->graphicsCandleView->setLayout(layout);
}

void shares::updateLineChart() {
    QLineSeries *lineSeries = new QLineSeries();
    QScatterSeries *scatterSeries = new QScatterSeries();
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(8.0);
    scatterSeries->setColor(QColor(173, 216, 230));

    QSqlDatabase dbq = QSqlDatabase::addDatabase("QSQLITE");
    dbq.setDatabaseName("candles.db");
    if (!dbq.open()) {
        qDebug() << "Unable to open the database: " << dbq.lastError().text();
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT close_units, close_nano, time_seconds FROM candles");
    if (!query.exec()) {
        showError("Unable to make the correct query: " + query.lastError().text());
        return;
    }

    while (query.next()) {
        qreal close = query.value("close_units").toDouble() + query.value("close_nano").toDouble() / 1e9;
        qint64 timestamp = query.value("time_seconds").toLongLong();
        QDateTime time = QDateTime::fromSecsSinceEpoch(timestamp);
        qreal timeValue = time.toMSecsSinceEpoch();
        lineSeries->append(timeValue, close);
        scatterSeries->append(timeValue, close);
    }

    QAreaSeries *areaSeries = new QAreaSeries(lineSeries);
    areaSeries->setColor(QColor(173, 216, 230, 100));
    areaSeries->setBorderColor(QColor(156, 183, 193, 200));

    QBarSeries *barSeries = new QBarSeries();
    barSeries->setBarWidth(0.5);

    QBarSet *barSet = new QBarSet("Volume");
    barSet->setColor(QColor(146, 193, 210, 200));
    barSet->setBorderColor(QColor(139, 184, 201, 200));


    QSqlQuery volumeQuery;
    volumeQuery.prepare("SELECT volume, time_seconds FROM candles");
    if (!volumeQuery.exec()) {
        showError("Unable to make the correct query: " + volumeQuery.lastError().text());
        return;
    }

    while (volumeQuery.next()) {
        qreal volume = volumeQuery.value("volume").toDouble();
        qint64 timestamp = volumeQuery.value("time_seconds").toLongLong();
        QDateTime time = QDateTime::fromSecsSinceEpoch(timestamp);
        qreal timeValue = time.toMSecsSinceEpoch();
        *barSet << volume;
        barSeries->append(barSet);
    }

    QChart *lineChart = new QChart();
    lineChart->addSeries(areaSeries);
    lineChart->addSeries(scatterSeries);
    lineChart->addSeries(barSeries);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM.yyyy<br>hh:mm:ss");
    axisX->setTitleText("Time");
    axisX->setLinePenColor(QColor("#d3d2d0"));
    axisX->setGridLinePen(QColor("#d3d2d0"));
    //axisX->setLabelsAngle(-45);
    lineChart->addAxis(axisX, Qt::AlignBottom);
    areaSeries->attachAxis(axisX);
    scatterSeries->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Price");
    axisY->setLinePenColor(QColor("#d3d2d0"));
    axisY->setGridLinePen(QColor("#d3d2d0"));
    //axisY->setLabelsAngle(-45);
    lineChart->addAxis(axisY, Qt::AlignLeft);
    areaSeries->attachAxis(axisY);
    scatterSeries->attachAxis(axisY);

    lineChart->legend()->setVisible(false);

    QChartView *lineChartView = new QChartView(lineChart);
    lineChartView->setRubberBand(QChartView::HorizontalRubberBand);
    lineChartView->setRenderHint(QPainter::Antialiasing);
    lineChart->setBackgroundBrush(QBrush(QColor("#ebeae7")));

    QVBoxLayout *lineLayout = new QVBoxLayout();
    lineLayout->addWidget(lineChartView);
    ui->graphicsLineView->setLayout(lineLayout);

    connect(scatterSeries, &QScatterSeries::hovered, this, &shares::showTooltip);
}

void shares::showTooltip(const QPointF &point, bool state) {
    if (state) {
        QString tooltipStyle = "QToolTip {"
                               "    color: black;"
                               "    background-color: #89B9CA;"
                               "    border: 1px solid #9CB7C1;"
                               "}";
        qApp->setStyleSheet(tooltipStyle);

        QToolTip::showText(QCursor::pos(), QString("Time: %1\nPrice: %2")
                                            .arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("dd.MM.yyyy hh:mm:ss"))
                                            .arg(point.y()));
    } else {
        qApp->setStyleSheet("");
        QToolTip::hideText();
    }
}

void shares::showTooltipCandle(bool status, QCandlestickSet *set) {
    if (status) {
        QString tooltipText = QString("Open: %1\nHigh: %2\nLow: %3\nClose: %4\nTime: %5")
                .arg(set->open())
                .arg(set->high())
                .arg(set->low())
                .arg(set->close())
                .arg(QDateTime::fromMSecsSinceEpoch(set->timestamp()).toString("dd.MM.yyyy hh:mm:ss"));
        QString tooltipStyle;
        if (set->close() < set->open()) {
            tooltipStyle = "QToolTip {"
                         "    color: black;"
                         "    background-color: #da4647;"
                         "    border: 3px solid #c34647;"
                         "}";
        } else {
            tooltipStyle = "QToolTip {"
                         "    color: black;"
                         "    background-color: #41a684;"
                         "    border: 3px solid #419484;"
                         "}";
        }
        qApp->setStyleSheet(tooltipStyle);
        QToolTip::showText(QCursor::pos(), tooltipText, this);
    } else {
        qApp->setStyleSheet("");
        QToolTip::hideText();
    }
}

void shares::clearDatabase() {
    sqlite3* db;
    int rc = sqlite3_open("candles.db", &db);
    if (rc == SQLITE_OK) {
        std::string sqlStatement = "DELETE FROM candles";
        char* errMsg = 0;
        rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            showError("SQL error: " + QString::fromStdString(errMsg));
            sqlite3_free(errMsg);
        }
        sqlite3_close(db);
    } else {
        showError("Can't open database: " + QString::number(rc));
    }
}

void shares::clearChart() {
    QLayout *layout = ui->graphicsCandleView->layout();
    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }

    layout = ui->graphicsLineView->layout();
    if (layout != nullptr) {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layout;
    }
}

void shares::updateDateTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->dateLabel->setText(currentDateTime.toString("dddd, MMMM d"));
    ui->timeLabel->setText(currentDateTime.toString("h:mm:ss"));
}