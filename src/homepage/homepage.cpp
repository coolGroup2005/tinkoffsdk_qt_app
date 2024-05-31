#include "homepage/homepage.h"
#include <QDebug> 
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <cmath>
#include <utility>
#include <string>
#include <sqlite3.h>
#include <filesystem>
#include <algorithm>


ShareInfo::ShareInfo(std::string name, std::string figi, unsigned int trading_status): 
                    name(name), 
                    figi(figi), 
                    trading_status(trading_status){};

ShareInfo::ShareInfo(std::string name, std::string figi, unsigned int trading_status, 
                    std::string currency, MoneyValue nominal): 
                    name(name), 
                    figi(figi), 
                    trading_status(trading_status), 
                    currency(currency), 
                    nominal(nominal){};

std::ostream& operator<< (std::ostream& ss, const ShareInfo& shareParam)
{
    ss << "Company: " << shareParam.name << '\n' << "Figi: " << shareParam.figi << '\n' << "Trading status: " << shareParam.trading_status << '\n';
    return ss;
}

ShareInfo getShareInfo(InvestApiClient& client, std::string& figi)
{
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto answerInstruments = instrumentService->GetInstrumentBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", figi);
    auto answerReply = dynamic_cast<InstrumentResponse*>(answerInstruments.ptr().get());
    // std::cout << answerReply->instrument().figi();

    std::string name = answerReply->instrument().name();
    unsigned int tradingStatus = answerReply->instrument().trading_status();

    ShareInfo share {name, figi, tradingStatus};
    return share;
}




void clearDatabaseStatistics() {
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("statistics.db", &db);

    if (rc == SQLITE_OK) {
        std::string sqlStatement = "DELETE FROM statistics";
        rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            std::cout << "Database cleared" << std::endl;
        }
        sqlite3_close(db);
    } else {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }
}


float getShareChange(int& intervalType, std::string& figi) {

    // std::cout << "aaaaaaa\n";

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::time_t dateFromToTime;

    switch (intervalType) {
        case 0: {
            auto dateFrom = now - std::chrono::hours(24);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        case 1: {
            auto dateFrom = now - std::chrono::hours(24 * 7);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        case 2: {
            auto dateFrom = now - std::chrono::hours(24 * 30);
            dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);
            break;
        }
        default: {
            std::cerr << "Invalid interval type" << std::endl;
            return -1;
        }
    }

    // std::cout << "bbbbbbbb\n";


    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    // CandleInterval interval = CandleInterval::CANDLE_INTERVAL_UNSPECIFIED;
    // interval = CandleInterval::CANDLE_INTERVAL_DAY;
    

    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    if (!marketdata) {
        qDebug() << "error marketdata";
        return -1;
    }
    auto candlesServiceReply = marketdata->GetCandles(figi, dateFromToTime, 0, currentTime, 0, CandleInterval::CANDLE_INTERVAL_DAY);

    auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());
    if (!response) {
        qDebug() << "error response";
        return -1;
    }

    if (response->candles_size() == 0) {
        qDebug() << "No candles found in the response";
        return -1; 
    }

    auto firstCandle = response->candles(0);
    auto lastCandle = response->candles(response->candles_size() - 1);

    auto open_units = firstCandle.open().units();
    auto open_nano = firstCandle.open().nano();
    auto close_units = lastCandle.close().units();
    auto close_nano = lastCandle.close().nano();

    if (!open_units && !open_nano) {
        qDebug() << "Invalid open units or nano";
        return -1;
    }

    if (!close_units && !close_nano) {
        qDebug() << "Invalid close units or nano";
        return -1;
    }

    float total_open = open_units + open_nano / (std::pow(10, std::to_string(open_nano).length()));
    float total_close = close_units + close_nano / (std::pow(10, std::to_string(close_nano).length()));

    float x = (total_close - total_open) * 100 / total_open;
    return x;

        // std::string volume = std::to_string(candle.volume());
    //     auto open_units = response->candles(0).open().units();
    //     auto open_nano = response->candles(0).open().nano();

    //     if (!open_units || !open_nano) {
    //         qDebug() << "error open_units";
    //         return -1;
    //     }


    // auto close_units = response->candles(response->candles_size() - 1).close().units();
    // auto close_nano = response->candles(response->candles_size() - 1).close().nano();

    // if (!close_nano || !close_units) {
    //     qDebug() << "error close_nano";
    //     return -1;
    // }

    // float total_open = open_units + open_nano/(std::pow(10, std::to_string(open_nano).length()));
    // float total_close = close_units + close_nano/(std::pow(10, std::to_string(close_nano).length()));


    // // std::cout << total_open << std::endl;
    // // std::cout << total_close << std::endl;
    // // std::cout << std::to_string(close_nano).length() << std::endl;
    // // std::cout << "open/close " << open_units << '\t' << close_units << '\n';
    // // std::cout << "open/close " << open_nano << '\t' << close_nano << '\n';


    // float x = (total_close - total_open) * 100 / total_open;
    // // std::cout << x << std::endl;
    // return x;
    
}

SharesVector getAllSharesWithChange(InvestApiClient& client, int& interval)
{
    // auto answerInstruments = instrumentService->GetInstrumentBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", figi);

    // auto answerShares = instrumentService->ShareBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", figi);
    // auto answerShareReply = dynamic_cast<ShareResponse*>(answerShares.ptr().get());
    // std::cout << answerReply->instrument().figi();

    // unsigned int tradingStatus = answerShareReply->instrument().trading_status();
    // std::string name = answerShareReply->instrument().name();
    // std::string currency = answerShareReply->instrument().currency();
    // MoneyValue nominal = answerShareReply->instrument().nominal();
    // nominal.units();
    // ShareInfo share {name, figi, tradingStatus, currency, nominal};

    // std::cout << share.name << '\t' << share.currency <<  std::endl;
    // return share;


    SharesVector allShares;
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));

    auto answerShares = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply = dynamic_cast<SharesResponse*>(answerShares.ptr().get());

    std::cout << answerShareReply->instruments_size();

    for (int i = 0; i < answerShareReply->instruments_size() - 1; i++) {

        unsigned int tradingStatus = answerShareReply->instruments(i).trading_status();
        // std::cout << i << "trading status: " << tradingStatus << std::endl;

        if (tradingStatus == 5) {
            std::string name =  answerShareReply->instruments(i).name();
            std::string figi =  answerShareReply->instruments(i).figi();
            std::string currency = answerShareReply->instruments(i).currency();
            MoneyValue nominal =  answerShareReply->instruments(i).nominal();
            ShareInfo share {name, figi, tradingStatus, currency, nominal};

            try {
                float shareChange = getShareChange(interval, share.figi);
                // std::cout << share.figi << '\t' << share.name << '\t' << shareChange << '\n';
                auto pair = std::make_pair(share, shareChange);
                allShares.push_back(pair);
            } catch (...) {
                std::cerr << "error getAllSharesWithChange " << '\n';
                continue;
            }
        }

    // std::cout << allShares.size();
    }


    // for (int i = 0; i < allShares.size(); ++i) {
    //     std::cout << allShares[i].first.name << '\t' << allShares[i].second << '\n';
    // }
    clearDatabaseStatistics();
    return allShares;
    
}




void insertStatisticsIntoDatabase(SharesVector& sharesVector) {    
    // Sort the sharesVector by priceChange in ascending order
    std::sort(sharesVector.begin(), sharesVector.end(), 
            [](const std::pair<ShareInfo, float>& a, const std::pair<ShareInfo, float>& b) {
                // bool i = a.second < b.second;
                // std::cout << a.second << " " << b.second << i << std::endl;
                return a.second < b.second;
            });

    for (const auto& sharePair : sharesVector) {
        std::cout << sharePair.first.figi << " " << sharePair.second << std::endl;
    }

    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("statistics.db", &db);

    std::string sqlStatement = "CREATE TABLE IF NOT EXISTS statistics ("
                               "ID INTEGER PRIMARY KEY  AUTOINCREMENT, "
                               "company_name TEXT, "
                               "company_figi TEXT, "
                               "total_price_change REAL);";
    rc = sqlite3_exec(db, sqlStatement.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    std::cout << "Database created" << std::endl;

    sqlite3_stmt* stmt;
    sqlStatement = "INSERT INTO statistics (company_name, company_figi, total_price_change) VALUES (?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sqlStatement.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    } else {
        for (const auto& sharePair : sharesVector) {
            const ShareInfo& share = sharePair.first;
            float priceChange = sharePair.second;

            sqlite3_bind_text(stmt, 1, share.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, share.figi.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 3, priceChange);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
                break;
            }
            sqlite3_reset(stmt); // Reset the statement for the next iteration
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    std::cout << "Inserted all" << std::endl;
}



std::vector<ShareInfo> parseFigi()
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
    auto accountList = accountService->GetAccounts();
    auto accountReply = dynamic_cast<GetAccountsResponse*>(accountList.ptr().get());
    // std::cout << accountReply->DebugString();

    auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
    auto getPortfolio = operationService->GetPortfolio(accountReply->accounts(accountReply->accounts_size() - 1).id(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioReply = dynamic_cast<PortfolioResponse*>(getPortfolio.ptr().get());
    auto PositionsList = portfolioReply->positions();

    std::vector<ShareInfo> sharesList;
    for (size_t i = 0; i < PositionsList.size(); i++)
    {
        std::string figi = portfolioReply->positions(i).figi();
        sharesList.push_back(getShareInfo(client, figi));
    }

    return sharesList;
}


std::string formatTradingStatus(unsigned int statusId)
{
    switch (statusId)
    {
        case 0:
            return "STATUS UNSPECIFIED";
        case 1:
            return "NOT AVAILABLE FOR TRADING";
        case 2:
            return "OPENING PERIOD";
        case 3:
            return "CLOSING PERIOD";
        case 4:
            return "BREAK IN TRADING";
        case 5:
            return "NORMAL TRADING";
        case 6:
            return "CLOSING AUCTION";
        case 7:
            return "DARK POOL AUCTION";
        case 8:
            return "DISCRETE AUCTION";
        case 9:
            return "OPENING AUCTION PERIOD";   
        case 10:
            return "TRADING AT CLOSING AUCTION PRICE";
        case 11:
            return "SESSION ASSIGNED";
        case 12:
            return "SESSION CLOSE";
        case 13:
            return "SESSION OPEN";
        case 14:
            return "DEALER NORMAL TRADING";
        case 15:
            return "DEALER BREAK IN TRADING";    
        case 16:
            return "DEALER NOT AVAILABLE FOR TRADING";
        default:
            return "ERROR";
    }
}