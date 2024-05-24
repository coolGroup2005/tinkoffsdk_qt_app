#include "homepage/homepage.h"

#include <string>
#include <vector>

ShareInfo::ShareInfo(std::string name, std::string figi, unsigned int trading_status): name(name), figi(figi), trading_status(trading_status){};


std::ostream& operator<< (std::ostream& ss, const ShareInfo& shareParam)
{
    ss << "Company: " << shareParam.name << '\n' << "Figi: " << shareParam.figi << '\n' << "Trading status: " << shareParam.trading_status << '\n';
    return ss;
}

ShareInfo getShareInfo(InvestApiClient& client, std::string& figi)
{
    auto instrumentService = std::dynamic_pointer_cast<Instruments>(client.service("instruments"));
    auto answerInstruments = instrumentService->GetInstrumentBy(InstrumentIdType::INSTRUMENT_ID_TYPE_FIGI, "", "BBG004730JJ5");
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
    auto getPortfolio = operationService->GetPortfolio(accountReply->accounts(accountReply->accounts_size() - 1).id(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioReply = dynamic_cast<PortfolioResponse*>(getPortfolio.ptr().get());
    auto virtualPositionsList = portfolioReply->virtual_positions();

    std::vector<ShareInfo> sharesList;
    for (size_t i = 0; i < virtualPositionsList.size(); i++)
    {
        std::string figi = portfolioReply->virtual_positions(i).figi();
        sharesList.push_back(getShareInfo(client, figi));
    }

    return sharesList;
}