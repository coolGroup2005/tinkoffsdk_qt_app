#ifndef __HOMEPAGE_H__
#define __HOMEPAGE_H__
#include <iostream>
#include <vector>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"

#include <string>
#include <vector>
#include <QString>


struct ShareInfo
{
    std::string name;
    std::string figi;
    std::string trading_status;

    ShareInfo(std::string, std::string, unsigned int);
    ShareInfo(std::string, std::string, unsigned int, std::string, MoneyValue);

};



ShareInfo getShareInfo(InvestApiClient&, std::string&);

std::vector<ShareInfo> parseFigi();
std::string formatTradingStatus(unsigned int);
QString accountsInfoText();

std::vector<ShareInfo> parseTest();


struct AccountInfo
{
    std::string name;
    std::string id;
    std::string totalValue;
    std::string relYield;

    AccountInfo(std::string, std::string, std::string, std::string);
};
    std::vector<AccountInfo> getAccountInfo();

#endif // __HOMEPAGE_H__