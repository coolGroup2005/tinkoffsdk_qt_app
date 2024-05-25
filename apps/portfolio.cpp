// #include "investapiclient.h"
// #include "usersservice.h"
// #include "operationsservice.h"

// int main(int argc, char *argv[])
// {
//     InvestApiClient client("invest-public-api.tinkoff.ru:443", getenv("TOKEN"));

//     auto operationService = std::dynamic_pointer_cast<Operations>(client.service("operations"));
//     // auto operationsStreamService = std::dynamic_pointer_cast<OperationsStreamService>(client.service("operationsstreamservice")); // самойлов его не сделал)
//     auto accountService = std::dynamic_pointer_cast<Users>(client.service("users"));

//     auto accountListOfID = accountService->GetAccounts();
//     auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
//     auto accountID12 = accountID1->accounts(0).id();

//     auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
//     auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
//     auto portfolioValue = portfolioAns->total_amount_portfolio().units();

//     std::cout << portfolioValue << " " << portfolioAns->total_amount_portfolio().currency() << '\n';
// }

#include "portfolio.h"
#include <QVBoxLayout>
#include <cstdlib>

Portfolio::Portfolio(QWidget *parent) : QWidget(parent)
{
    balanceLabel = new QLabel(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(balanceLabel);
    setLayout(layout);

    token = getenv("TOKEN");
    client = new InvestApiClient("sandbox-invest-public-api.tinkoff.ru:443", token.toStdString()); //sandbox-

    updateBalance();
}

void Portfolio::updateBalance()
{
    auto operationService = std::dynamic_pointer_cast<Operations>(client->service("operations"));
    auto accountService = std::dynamic_pointer_cast<Users>(client->service("users"));

    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());
    auto accountID12 = accountID1->accounts(0).id();

    auto portfolioRequest = operationService->GetPortfolio(accountID12, PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    QString balanceText = QString::number(portfolioValue) + " " + QString::fromStdString(portfolioAns->total_amount_portfolio().currency());
    balanceLabel->setText(balanceText);
}
