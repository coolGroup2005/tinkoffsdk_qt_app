#ifndef __STATISTICS_H__
#define __STATISTICS_H__
#include <iostream>
#include <vector>
#include <QObject>
#include <QStringListModel>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"
#include "../homepage/homepage.h"


typedef std::vector<std::pair<ShareInfo, float>> SharesVector;

class StatisticsManager : public QObject
{
    Q_OBJECT
public:
    explicit StatisticsManager(QObject *parent = nullptr);
    void updateStatistics(int interval, QStringListModel* topGainersModel, QStringListModel* topLosersModel, bool cropped);

signals:
    void statisticsUpdated();
};


SharesVector getAllSharesWithChange(InvestApiClient&, int&, bool);
std::vector<std::pair<std::string, float>> getTopFromDb(std::string type);
float getShareChange(std::string&, std::time_t&, std::time_t&); 
void clearDatabaseStatistics();
void insertStatisticsIntoDatabase(SharesVector&);
bool isWeekend(std::time_t time);
bool isOutsideWorkingHours(std::time_t time);
std::time_t adjustToWorkingTime(std::time_t time);
#endif // __STATISTICS_H__