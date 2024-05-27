#ifndef SHARES_H
#define SHARES_H

#include "investapiclient.h"
#include "sandboxservice.h"
#include "marketdataservice.h"
#include <QTimer>
#include <QDialog>

namespace Ui {
class shares;
}

class shares : public QDialog
{
    Q_OBJECT

public:
    explicit shares(QWidget *parent = nullptr, const std::string& figi = "", const std::string& stockName = "");
    ~shares();

private slots:
    void updateDateTime();
    void clearChart();
    void clearDatabase();
    void fetchCandlestickData();
    void updateChart();

private:
    Ui::shares *ui;
    QTimer *timer;
    std::string figi;
    std::string stockName;
};

#endif // SHARES_H
