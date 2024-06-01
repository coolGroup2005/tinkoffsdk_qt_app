#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__
#include <iostream>
#include <QString>
#include <QStringListModel>
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
    unsigned int trading_status;

    ShareInfo(std::string, std::string, unsigned int);
};

ShareInfo getShareInfo(InvestApiClient&, std::string&);
std::vector<ShareInfo> parseFigi(const QString& token);
std::string formatTradingStatus(unsigned int);

#endif // __EXAMPLE_H__