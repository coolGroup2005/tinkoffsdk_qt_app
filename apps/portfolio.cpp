#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"

int main(int argc, char *argv[])
{
    InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

    auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
    // auto operationsStreamService = std::dynamic_pointer_cast<OperationsStreamService>(client.service("operationsstreamservice")); // самойлов его не сделал)
    auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));

    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
    auto accountID12 = accountID1->accounts(0).id();

    auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    std::cout << portfolioValue << " " << portfolioAns->total_amount_portfolio().currency() << '\n';
}
