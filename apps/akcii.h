#ifndef AKCII_H
#define AKCII_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class akcii;
}

class akcii : public QDialog
{
    Q_OBJECT

public:
    explicit akcii(QWidget *parent = nullptr, const std::string& figi = "", const std::string& stockName = "");
    ~akcii();

private slots:
    void updateDateTime();
    void clearChart();
    void clearDatabase();
    void fetchCandlestickData();
    void updateChart();

private:
    Ui::akcii *ui;
    QTimer *timer;
    std::string figi;
    std::string stockName;
};

#endif // AKCII_H

