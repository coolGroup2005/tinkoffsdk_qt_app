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


QString accountsInfoText()
{
    std::vector<AccountInfo> accountsInfos = getAccountInfo();
    QString outputText = "";
    for (AccountInfo acc: accountsInfos)
    {
        outputText += acc.name + acc.id + acc.totalValue + acc.relYield;
    }
    return outputText;
}


ShareInfo::ShareInfo(std::string name, std::string figi, unsigned int trading_status): name(name), figi(figi), trading_status(formatTradingStatus(trading_status)){};


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


std::vector<ShareInfo> parseFavFigi()
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto instrumentAnswer = instrumentService->GetFavorites();
    auto favouritesList =  dynamic_cast<GetFavoritesResponse*>(instrumentAnswer.ptr().get());

    std::vector<ShareInfo> sharesList;
    for (size_t i = 0; i < favouritesList->favorite_instruments_size(); ++i)
    {
        std::string figi = favouritesList->favorite_instruments(i).figi();
        sharesList.push_back(getShareInfo(client, figi));
    }
    return sharesList;
}


std::vector<ShareInfo> parseTest()
{
    std::vector<ShareInfo> sharesList;
    sharesList.push_back({"Контора лапочек", "ASD742964EDR", 5});
    sharesList.push_back({"Жмых Эйрлайнс", "APG543789BDK", 12});
    sharesList.push_back({"ООО Зеленоглазое такси", "WKV349048HCK", 14});
    sharesList.push_back({"Таганрогская фабрика стульев", "WKC492840FPK", 1});
    sharesList.push_back({"НИУ ВШИ", "RLA920548RIS", 6});
    sharesList.push_back({"Рога и копыта", "JFK530593EJF", 2});
    sharesList.push_back({"Галина", "SIL830204THC", 7});
    sharesList.push_back({"Pythonzilla Inc.", "LFH673839FHJ", 0});
    sharesList.push_back({"Мебельная фабрика стильные кухни", "FUS948285UDJ", 6});
    sharesList.push_back({"Kakao M", "YSP732957USL", 3});
    sharesList.push_back({"Колыбелькин", "DKS749275HFI", 2});
    sharesList.push_back({"Уют Обед", "HFK582020WIF", 5});
    sharesList.push_back({"Polymetal", "DKS682024HDD", 9});
    sharesList.push_back({"Лаборатория Лос-Аламоса", "SLF402058EOE", 7});
    sharesList.push_back({"ЗИЛ", "NAO362603OPS", 13});
    sharesList.push_back({"Эн+", "OCP239030RKD", 2});
    sharesList.push_back({"Татнефть", "FKL573924PGF", 8});
    sharesList.push_back({"АО Красногорский завод им. С.А. Зверева", "DJL759274DSJ", 4});
    return sharesList;
}