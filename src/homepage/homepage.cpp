#include "homepage/homepage.h"

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <cmath>

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

ShareInfo getShareInfoStatistics(InvestApiClient& client, std::string& figi)
{
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    // auto answerInstruments = instrumentService->GetInstrumentBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", figi);

    auto answerShares = instrumentService->ShareBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", figi);
    auto answerShareReply = dynamic_cast<ShareResponse*>(answerShares.ptr().get());
    // std::cout << answerReply->instrument().figi();

    unsigned int tradingStatus = answerShareReply->instrument().trading_status();
    std::string name = answerShareReply->instrument().name();
    std::string currency = answerShareReply->instrument().currency();
    MoneyValue nominal = answerShareReply->instrument().nominal();
    nominal.units();
    ShareInfo share {name, figi, tradingStatus, currency, nominal};

    // std::cout << share.name << '\t' << share.currency <<  std::endl;
    // return share;
    

    auto answerShares1 = instrumentService->Shares(INSTRUMENT_STATUS_BASE);
    auto answerShareReply1 = dynamic_cast<SharesResponse*>(answerShares1.ptr().get());

    // instruments(0)
    // std::cout << answerShareReply->instruments_size();
    // std::cout << answerShareReply->instruments(0).name();

    // tinkoff::public_::invest::api::contract::v1::SharesResponse
    for (int i = 0; i < answerShareReply1->instruments_size() - 1; i++) {
        std::cout << answerShareReply1->instruments(i).figi() << '\t' << answerShareReply1->instruments(i).name() << '\n';
    }

    return share ;
    
}


float getShareChange(int& intervalType, std::string& figi) {

    std::cout << "aaaaaaa\n";

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

    std::cout << "bbbbbbbb\n";


    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    CandleInterval interval = CandleInterval::CANDLE_INTERVAL_UNSPECIFIED;
    interval = CandleInterval::CANDLE_INTERVAL_DAY;
    

    auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
    auto candlesServiceReply = marketdata->GetCandles(figi, dateFromToTime, 0, currentTime, 0, interval);

    auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());

    // std::string volume = std::to_string(candle.volume());
    auto open_units = response->candles(0).open().units();
    auto open_nano = response->candles(0).open().nano();

    auto close_units = response->candles(response->candles_size() - 1).close().units();
    auto close_nano = response->candles(response->candles_size() - 1).close().nano();

    float total_open = open_units + open_nano/(std::pow(10, std::to_string(open_nano).length()));
    float total_close = close_units + close_nano/(std::pow(10, std::to_string(close_nano).length()));

    std::cout << total_open << std::endl;
    std::cout << total_close << std::endl;
    std::cout << std::to_string(close_nano).length() << std::endl;
    std::cout << "open/close " << open_units << '\t' << close_units << '\n';
    std::cout << "open/close " << open_nano << '\t' << close_nano << '\n';

    float x = (total_close - total_open) * 100 / total_open;
    std::cout << x << std::endl;
    return x;
    
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