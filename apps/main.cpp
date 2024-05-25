#include <QApplication>
#include "mainwindow.h"
// #include "invest/example.h"
#include "homepage/homepage.h"

#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"

int main(int argc, char *argv[])
{
    InvestApiClient client("sandbox-invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox-

    auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
    // auto operationsStreamService = std::dynamic_pointer_cast<OperationsStreamService>(client.service("operationsstreamservice")); // самойлов его не сделал)
    auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
    auto sandboxService = std::dynamic_pointer_cast<Sandbox>(client.service("sandbox"));

    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
    auto accountID12 = accountID1->accounts(0).id();

    std::cout << accountID12 << "\n";

    // auto sandboxPayInRequest = sandboxService->SandboxPayIn(accountID12, "RUB", 100500, 0);

    auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    // auto getMarginAttributesRequest = accountService->GetMarginAttributes(accountID12);
    std::cout << portfolioRequest.ptr()->DebugString() << '\n';
    // auto getMarginAttributesResponse = dynamic_cast<GetMarginAttributesResponse*>(getMarginAttributesRequest.ptr().get());

    std::cout << portfolioValue << " " << portfolioAns->total_amount_portfolio().currency() << '\n';
}

// #include <QApplication>
// #include "mainwindow.h"
// // #include "invest/example.h"

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     MainWindow mainWindow;
//     mainWindow.show();

//     return app.exec();
// }
