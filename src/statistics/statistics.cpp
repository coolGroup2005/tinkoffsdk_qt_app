// #include "shares/shares.h"
// #include "ui_shares.h"
// #include <chrono>
// #include <ctime>


// int getShareChange(int& intervalType, std::string& figi) {

//     auto now = std::chrono::system_clock::now();
//     std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
//     auto dateFrom;
//     switch (intervalType) {
//         case 0 :
//             dateFrom =  now - std::chrono::hours(24);
//         case 1:
//             dateFrom = now - std::chrono::hours(24 * 7);
//         case 2:
//             dateFrom = now - std::chrono::hours(24 * 7 * 30);
//     }

//     std::time_t dateFromToTime = std::chrono::system_clock::to_time_t(dateFrom);

//     InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

//     CandleInterval interval = CandleInterval::CANDLE_INTERVAL_UNSPECIFIED;
//     interval = CandleInterval::CANDLE_INTERVAL_DAY;
    

//     auto marketdata = std::dynamic_pointer_cast<MarketData>(client.service("marketdata"));
//     auto candlesServiceReply = marketdata->GetCandles(figi, dateFromToTime, 0, currentTime, 0, interval);

//     auto response = dynamic_cast<GetCandlesResponse*>(candlesServiceReply.ptr().get());
//     if (!response) {
//         showError("Failed to retrieve candle data.");
//         return;
//     }

//     // std::string volume = std::to_string(candle.volume());
//     auto open_units = response->candles(0).open().units();
//     auto close_units = response->candles(response->candles_size() - 1).close().units();

//     return (close_units - open_units) * 100 / open_units
    
// }

