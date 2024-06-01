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
#include <QDebug> // for finding segmentation fault

Portfolio::Portfolio(QWidget *parent) : QWidget(parent)
{
    balanceLabel = new QLabel(this);
    yieldLabel = new QLabel(this);
    accountComboBox = new QComboBox(this);
    portfolioTableView = new QTableView(this);
    portfolioModel = new QStandardItemModel(this);
    portfolioModel->setHorizontalHeaderLabels({"Ticker", "Name", "Quantity", "Current Price", "Value"});

    virtualPortfolioTableView = new QTableView(this);
    virtualPortfolioModel = new QStandardItemModel(this);
    virtualPortfolioModel->setHorizontalHeaderLabels({"Ticker", "Name", "Quantity", "Current Price", "Value"});

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accountComboBox);
    layout->addWidget(balanceLabel);
    layout->addWidget(yieldLabel);
    layout->addWidget(portfolioTableView);
    layout->addWidget(new QLabel("Virtual Positions", this));
    layout->addWidget(virtualPortfolioTableView);
    setLayout(layout);

    token = getenv("TOKEN");
    client = new InvestApiClient("invest-public-api.tinkoff.ru:443", token.toStdString()); // sandbox-

    auto accountService = std::dynamic_pointer_cast<Users>(client->service("users"));

    if (!accountService) {
        qDebug() << "Failed to get Users service";
        return;
    }

    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse*>(accountListOfID.ptr().get());

    if (!accountID1) {
        qDebug() << "Failed to get accounts list";
        return;
    }

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
    virtualPortfolioTableView->setModel(virtualPortfolioModel);
}

void Portfolio::updateBalance(const QString& accountId)
{

    if (!client) {
        qDebug() << "InvestApiClient is not initialized";
        return;
    }

    auto operationService = std::dynamic_pointer_cast<Operations>(client->service("operations"));

    if (!operationService) {
        qDebug() << "Failed to get Operations service";
        return;
    }

    auto instrumentsService = std::dynamic_pointer_cast<Instruments>(client->service("instruments"));

    if (!instrumentsService) {
        qDebug() << "Failed to get Instruments service";
        return;
    }

    auto portfolioRequest = operationService->GetPortfolio(accountId.toStdString(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse*>(portfolioRequest.ptr().get());

    if (!portfolioAns) {
        qDebug() << "Failed to get portfolio response";
        return;
    }

    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    balanceLabel->setText("Balance: " + QString::number(portfolioValue) + " " + QString::fromStdString(portfolioAns->total_amount_portfolio().currency()));

    portfolioModel->setRowCount(portfolioAns->positions_size() - 1); // не понял почему, но вроде воркает
    // auto portfolio1 = portfolioAns->positions(0);

    // std::cout << portfolioAns->positions(0) << '\n';

    for (int i = 0; i < portfolioAns->positions_size(); ++i) {
        const auto& position = portfolioAns->positions(i);

        // std::cout << portfolioRequest.ptr()->DebugString() << '\n';
        
        auto instrumentUID = position.instrument_uid();

        // if (position.instrument_type() == "currency"){
        //     auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
        //     auto instrumentResponse = dynamic_cast<CurrencyResponse*>(instrumentRequest.ptr().get());
        // }

        auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
        auto instrumentResponse = dynamic_cast<InstrumentResponse*>(instrumentRequest.ptr().get());

        // auto instrumentRequest = instrumentsService->ShareBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
        // auto shareResponse = dynamic_cast<ShareResponse*>(instrumentRequest.ptr().get());

        if (!instrumentResponse) {
            qDebug() << "Failed to get instrumentResponse";
            return;
        }

        // std::cout << "Я тут!" << '\n';
        // std::cout << "Я тут!" << '\n';


        auto instrumentInfo = instrumentResponse->instrument();
        auto instrumentName = instrumentInfo.name();
        auto instrumentTicker = instrumentInfo.ticker();

        // auto positionQuantity = position.quantity();
        // auto shareInfo = shareResponse->instrument();
        // auto shareName = shareInfo.name();
        // auto shareTicker = shareInfo.ticker();

        QList<QStandardItem*> rowItems;
        rowItems.append(new QStandardItem(QString::fromStdString(instrumentTicker)));
        rowItems.append(new QStandardItem(QString::fromStdString(instrumentName)));
        rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
        rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
        rowItems.append(new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency())));
        portfolioModel->appendRow(rowItems);
    }

    auto virtualPositions = portfolioAns->virtual_positions();

    virtualPortfolioModel->setRowCount(virtualPositions.size() - 1);

    for (int i = 0; i < virtualPositions.size(); ++i) {
        const auto& position = virtualPositions[i];

        auto instrumentUID = position.instrument_uid();

        auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
        auto instrumentResponse = dynamic_cast<InstrumentResponse*>(instrumentRequest.ptr().get());

        if (!instrumentResponse) {
            qDebug() << "Failed to get instrumentResponse";
            return;
        }

        auto instrumentInfo = instrumentResponse->instrument();
        auto instrumentName = instrumentInfo.name();
        auto instrumentTicker = instrumentInfo.ticker();

        QList<QStandardItem*> rowItems;
        rowItems.append(new QStandardItem(QString::fromStdString(instrumentTicker)));
        rowItems.append(new QStandardItem(QString::fromStdString(instrumentName)));
        rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
        rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
        rowItems.append(new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency())));
        virtualPortfolioModel->appendRow(rowItems);
    }

    auto yield = portfolioAns->expected_yield();

    double yieldValue = yield.units() + yield.nano() * 1e-9;
    yieldLabel->setText("Expected Yield: " + QString::number(yieldValue) + "%");
}

// void Portfolio::updateExpectedYield(const Quotation& yield)
// {
//     double yieldValue = yield.units() + yield.nano() * 1e-9;
//     yieldLabel->setText("Expected Yield: " + QString::number(yieldValue) + "%");
// }

// void Portfolio::updateVirtualPositions(const google::protobuf::RepeatedPtrField<VirtualPortfolioPosition>& virtualPositions)
// {
//     virtualPortfolioModel->setRowCount(virtualPositions.size());

//     for (int i = 0; i < virtualPositions.size(); ++i) {
//         const auto& position = virtualPositions[i];

//         QList<QStandardItem*> rowItems;
//         // rowItems.append(new QStandardItem(QString::fromStdString(position.name())));
//         rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
//         rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
//         rowItems.append(new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency())));
//         virtualPortfolioModel->appendRow(rowItems);
//     }
// }

void Portfolio::onAccountChanged(int index)
{
    QString accountId = accountComboBox->itemData(index).toString();
    updateBalance(accountId);
}
