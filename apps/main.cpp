// #include <QApplication>
// #include "mainwindow.h"
#include "homepage/homepage.h"
#include "statistics/statistics.h"

#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"
#include <ordersservice.h> // for adding shares into positions
#include <QApplication>
#include "mainwindow.h"
#include "marketdataservice.h"

#include "instrumentsservice.h"

#include "invest/example.h"

// int main(int argc, char *argv[])
// {
//     InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox-
//     InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox-

//     auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
//     // auto operationsStreamService = std::dynamic_pointer_cast<OperationsStreamService>(client.service("operationsstreamservice")); // самойлов его не сделал)
//     auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
//     auto sandboxService = std::dynamic_pointer_cast<Sandbox>(client.service("sandbox"));
//     auto ordersService = std::dynamic_pointer_cast<Orders>(client.service("orders"));

//     auto accountListOfID = accountService->GetAccounts();
//     auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
//     // auto accountID12 = accountID1->accounts(0).id();
//     auto accountID12 = sandboxService->GetSandboxAccounts().accountID(0);

//     std::cout << sandboxService->GetSandboxAccounts().ptr()->DebugString() << "\n";

//     // auto PostOrderRequest = sandboxService->PostSandboxOrder("BBG006L8G4H1", 1, 1000,0 ,tinkoff::public_::invest::api::contract::v1::OrderDirection::ORDER_DIRECTION_BUY, accountID12, tinkoff::public_::invest::api::contract::v1::OrderType::ORDER_TYPE_MARKET,"a6a015df-caf7-4ee1-bc54-3980de159bbf"); // не работает по выходным (ОЧЕВ) 962e2a95-02a9-4171-abd7-aa198dbe643a
//     // auto postOrderResponse = dynamic_cast<PostOrderResponse*>(PostOrderRequest.ptr().get());
//     // std::cout << "Я тут" << '\n';
//     // auto postOrderRequest = ordersService->PostOrder("BBG004730RP0", 2, 0, 0, tinkoff::public_::invest::api::contract::v1::OrderDirection::ORDER_DIRECTION_BUY, accountID12, tinkoff::public_::invest::api::contract::v1::OrderType::ORDER_TYPE_BESTPRICE, "123"); // не работает по выходным (ОЧЕВ)
//     // auto postOrderResponse = dynamic_cast<PostOrderResponse*>(postOrderRequest.ptr().get());
//     // std::cout << postOrderResponse->execution_report_status() << '\n';

//     auto replyOrder = sandboxService->PostSandboxOrder("BBG00PYL6D50", 100, 0, 0, ORDER_DIRECTION_BUY,
// accountID12, ORDER_TYPE_MARKET, "12341234123875");
//     std::cout << replyOrder.ptr()->DebugString() << '\n';

//     auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
//     auto sandboxPayInRequest = sandboxService->SandboxPayIn(accountID12, "RUB", 100500, 0);

//     auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);

//     auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
//     auto portfolioValue = portfolioAns->total_amount_portfolio().units();

//     auto getMarginAttributesRequest = accountService->GetMarginAttributes(accountID12);
//     std::cout << portfolioRequest.ptr()->DebugString() << '\n';
//     auto getMarginAttributesResponse = dynamic_cast<GetMarginAttributesResponse*>(getMarginAttributesRequest.ptr().get());

//     std::cout << portfolioValue << " " << portfolioAns->total_amount_portfolio().currency() << '\n';
// }



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
