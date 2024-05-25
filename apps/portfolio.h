#ifndef PORTFOLIOBALANCE_H
#define PORTFOLIOBALANCE_H

#include <QWidget>
#include <QLabel>
#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"

class Portfolio : public QWidget
{
    Q_OBJECT

public:
    explicit Portfolio(QWidget *parent = nullptr);
    void updateBalance();

private:
    QLabel *balanceLabel;
    InvestApiClient *client;
    QString token;
};

#endif // PORTFOLIOBALANCE_H

