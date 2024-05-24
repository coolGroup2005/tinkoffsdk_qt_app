#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__
#include <iostream>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"
#include <string>


struct ShareInfo
{
    std::string name;
    std::string figi;
    unsigned int trading_status;

    ShareInfo(std::string, std::string, unsigned int);
};

ShareInfo getShareInfo(InvestApiClient&, std::string&);
std::vector<ShareInfo> parseFigi();

#endif // __EXAMPLE_H__