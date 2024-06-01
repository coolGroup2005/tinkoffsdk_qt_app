#ifndef __STATISTICS_H__
#define __STATISTICS_H__
#include <iostream>
#include <vector>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"
#include "../homepage/homepage.h"


typedef std::vector<std::pair<ShareInfo, float>> SharesVector;


SharesVector getAllSharesWithChange(InvestApiClient&, int&);
std::vector<std::pair<std::string, float>> getTopGainers();
std::string formatTradingStatus(unsigned int);
float getShareChange(std::string&, std::time_t&, std::time_t&); 
void clearDatabaseStatistics();
void insertStatisticsIntoDatabase(SharesVector&);
bool isWeekend(std::time_t time);
bool isOutsideWorkingHours(std::time_t time);
std::time_t adjustToWorkingTime(std::time_t time);
#endif // __STATISTICS_H__