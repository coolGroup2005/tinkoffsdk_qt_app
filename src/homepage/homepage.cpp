#include "homepage/homepage.h"

AccountInfo::AccountInfo(std::string name, std::string id, std::string totalValue, std::string relYield): name(name), id(id), totalValue(totalValue), relYield(relYield){};

std::vector<AccountInfo> getAccountInfo()
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
    auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
    auto accountList = accountService->GetAccounts();
    auto accountReply = dynamic_cast<GetAccountsResponse*>(accountList.ptr().get());
    std::vector<AccountInfo> accountsInfo;
    for (int i = 0; i < accountReply->accounts_size(); ++i)
    {
        std::string accountName = accountReply->accounts(i).name() + '\n';
        std::string accountId = accountReply->accounts(i).id() + '\n';
        auto getPortfolio = operationService->GetPortfolio(accountReply->accounts(i).id(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
        auto portfolioReply = dynamic_cast<PortfolioResponse*>(getPortfolio.ptr().get());
        std::string totalValue = "The total value of the portfolio:\n" + std::to_string(portfolioReply->total_amount_portfolio().units()) + " " + portfolioReply->total_amount_portfolio().currency() + '\n';
        std::string yield = "Current relative portfolio yield:\n" + std::to_string(portfolioReply->expected_yield().units()) + "%";
        accountsInfo.push_back({accountName, accountId, totalValue, yield});
    }
    return accountsInfo;
}

ShareInfo::ShareInfo(std::string name, std::string figi, unsigned int trading_status): name(name), figi(figi), trading_status(trading_status){};


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


std::vector<ShareInfo> parseFigi()
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
    auto accountList = accountService->GetAccounts();
    auto accountReply = dynamic_cast<GetAccountsResponse*>(accountList.ptr().get());
    // std::cout << accountReply->DebugString();

    auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
// TODO: Take into account the fact that there are several accounts, consider doing a combobox to switch between them
    auto getPortfolio = operationService->GetPortfolio(accountReply->accounts(accountReply->accounts_size() - 1).id(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioReply = dynamic_cast<PortfolioResponse*>(getPortfolio.ptr().get());
    auto PositionsList = portfolioReply->positions(); // virtual_positions()

    std::vector<ShareInfo> sharesList;
    for (size_t i = 0; i < PositionsList.size(); i++)
    {
        std::string figi = portfolioReply->positions(i).figi(); // virtual_positions()
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