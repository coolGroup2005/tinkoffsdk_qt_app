// #include <QApplication>
// #include "mainwindow.h"
// // #include "invest/example.h"
// #include "homepage/homepage.h"

// #include "investapiclient.h"
// #include "usersservice.h"
// #include "operationsservice.h"
// #include "sandboxservice.h"
// #include <ordersservice.h>
// #include <ordersservice.h> // for adding shares into positions

// int main(int argc, char *argv[])
// {
//     InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN")); //sandbox-

//     auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
//     // auto operationsStreamService = std::dynamic_pointer_cast<OperationsStreamService>(client.service("operationsstreamservice")); // самойлов его не сделал)
//     auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));
//     auto sandboxService = std::dynamic_pointer_cast<Sandbox>(client.service("sandbox"));
//     auto ordersService = std::dynamic_pointer_cast<Orders>(client.service("orders"));

//     auto accountListOfID = accountService->GetAccounts();
//     auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
//     auto accountID12 = accountID1->accounts(0).id();

//     // std::cout << accountListOfID.ptr()->DebugString() << "\n";

//     auto getInfoRequest = accountService->GetInfo();
//     auto getInfoResponse = dynamic_cast<GetInfoResponse*>(getInfoRequest.ptr().get());
//     auto traderTariff = getInfoResponse->tariff();
//     std::cout << traderTariff;

//     // std::cout << "Я тут" << '\n';
//     // auto postOrderRequest = ordersService->PostOrder("BBG004730RP0", 2, 0, 0, tinkoff::public_::invest::api::contract::v1::OrderDirection::ORDER_DIRECTION_BUY, accountID12, tinkoff::public_::invest::api::contract::v1::OrderType::ORDER_TYPE_BESTPRICE, "123"); // не работает по выходным (ОЧЕВ)
//     // auto postOrderResponse = dynamic_cast<PostOrderResponse*>(postOrderRequest.ptr().get());
//     // std::cout << postOrderResponse->execution_report_status() << '\n';

//     // auto getOrdersRequest = ordersService->GetOrders(accountID12);
//     // std::cout << getOrdersRequest.ptr()->DebugString() << '\n';

//     // auto sandboxPayInRequest = sandboxService->SandboxPayIn(accountID12, "RUB", 100500, 0);
//     // auto OpenSandboxAccountRequest = sandboxService->OpenSandboxAccount();

//     // auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);

//     // auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
//     // auto portfolioValue = portfolioAns->total_amount_portfolio().units();

//     // // auto getMarginAttributesRequest = accountService->GetMarginAttributes(accountID12);
//     // std::cout << portfolioRequest.ptr()->DebugString() << '\n';
//     // // auto getMarginAttributesResponse = dynamic_cast<GetMarginAttributesResponse*>(getMarginAttributesRequest.ptr().get());

//     // std::cout << portfolioValue << " " << portfolioAns->total_amount_portfolio().currency() << '\n';
// }

#include <QApplication>
#include "mainwindow.h"
// #include "invest/example.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
