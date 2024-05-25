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
    accountComboBox = new QComboBox(this);
    portfolioTableView = new QTableView(this);
    portfolioModel = new QStandardItemModel(this);
    portfolioModel->setHorizontalHeaderLabels({"Ticker", "Name", "Quantity", "Current Price"});

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accountComboBox);
    layout->addWidget(balanceLabel);
    layout->addWidget(portfolioTableView);
    setLayout(layout);

    token = getenv("TOKEN");
    client = new InvestApiClient("invest-public-api.tinkoff.ru:443", token.toStdString()); // sandbox-

    auto accountService = std::dynamic_pointer_cast<Users>(client->service("users"));
    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());

    for (int i = 0; i < accountID1->accounts_size(); ++i) {
        auto account = accountID1->accounts(i);
        accountIds.push_back(account.id().c_str());
        accountComboBox->addItem(account.name().c_str(), QVariant(account.id().c_str()));
    }

    connect(accountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAccountChanged(int)));

    if (!accountIds.empty()) {
        updateBalance(accountIds[0]);
    }

    portfolioTableView->setModel(portfolioModel);
}

void Portfolio::updateBalance(const QString& accountId)
{
    auto operationService = std::dynamic_pointer_cast<Operations>(client->service("operations"));
    auto instrumentsService = std::dynamic_pointer_cast<Instruments>(client->service("instruments"));

    auto portfolioRequest = operationService->GetPortfolio(accountId.toStdString(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());
    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    balanceLabel->setText(QString::number(portfolioValue) + " " + QString::fromStdString(portfolioAns->total_amount_portfolio().currency()));

    portfolioModel->removeRows(0, portfolioModel->rowCount());

    for (int i = 0; i < portfolioAns->positions_size(); ++i) {
        const auto& position = portfolioAns->positions(i);

        auto instrumentUID = position.instrument_uid();
        auto instrumentRequest = instrumentsService->ShareBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
        auto shareResponse = dynamic_cast<ShareResponse*>(instrumentRequest.ptr().get());
        auto shareInfo = shareResponse->instrument();
        auto shareName = shareInfo.name();
        auto shareTicker = shareInfo.ticker();

        QList<QStandardItem*> rowItems;
        rowItems.append(new QStandardItem(QString::fromStdString(shareTicker)));
        rowItems.append(new QStandardItem(QString::fromStdString(shareName)));
        rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
        rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
        portfolioModel->appendRow(rowItems);
    }
}

void Portfolio::onAccountChanged(int index)
{
    QString accountId = accountComboBox->itemData(index).toString();
    updateBalance(accountId);
}
