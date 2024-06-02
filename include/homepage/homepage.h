#ifndef __HOMEPAGE__
#define __HOMEPAGE__
#include <iostream>
#include <QString>
#include <QStringListModel>#include <vector>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"


struct ShareInfo
{
    std::string name;
    std::string figi;

    std::string trading_status;

    std::string currency;
    MoneyValue nominal;

    ShareInfo(std::string, std::string, unsigned int);
    ShareInfo(std::string, std::string, unsigned int, std::string, MoneyValue);

};


ShareInfo getShareInfo(InvestApiClient&, std::string&);
std::vector<ShareInfo> parseFigi(const QString& token);
std::string formatTradingStatus(unsigned int);

#endif // __HOMEPAGE__