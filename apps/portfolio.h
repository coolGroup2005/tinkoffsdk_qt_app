#ifndef PORTFOLIOBALANCE_H
#define PORTFOLIOBALANCE_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"
#include <ordersservice.h> // for adding shares into positions

class Portfolio : public QWidget
{
    Q_OBJECT

public:
    explicit Portfolio(QWidget *parent = nullptr);
    void updateBalance(const QString& accountId);

private slots:
    void onAccountChanged(int index);

private:
    QLabel *balanceLabel;
    InvestApiClient *client;
    QString token;
    QComboBox *accountComboBox;
    std::vector<QString> accountIds;
};

#endif // PORTFOLIOBALANCE_H

