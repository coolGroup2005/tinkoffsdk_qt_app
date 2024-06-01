#include "portfolio.h"
#include <QVBoxLayout>
#include <cstdlib>
#include <QDebug> // for finding segmentation fault
#include <QHeaderView>

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

    updateButton = new QPushButton("Update", this);

    updateButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgb(193, 193, 193);"  
        "    border-radius: 15px;"  
        "    padding: 10px 20px;"  
        "}"
        "QPushButton:hover {"
        "    background-color: rgb(170, 170, 170);"  
        "}"
        "QPushButton:pressed {"
        "    background-color: rgb(150, 150, 150);"  
        "}"
    );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accountComboBox);
    layout->addWidget(balanceLabel);
    layout->addWidget(yieldLabel);
    layout->addWidget(portfolioTableView);
    layout->addWidget(new QLabel("Virtual Positions", this));
    layout->addWidget(virtualPortfolioTableView);
    layout->addWidget(updateButton);
    setLayout(layout);

    this->setStyleSheet(R"(
        QWidget {
            background-color: #ebeae7;
            color: #525252;
        }
        QLabel {
            color: #525252;
        }
        QLineEdit, QComboBox {
            background-color: #d3d2d0;
            border-radius: 15px;
            color: #525252;
            padding: 10px;
        }
        QHeaderView::section {
            background-color: lightgrey;
        }
        QComboBox::down-arrow {
            image: none;
        }
        QTableView {
            gridline-color: #b0b0af;
            background-color: #d3d2d0;
            border: 1px solid #d3d2d0;
            border-radius: 20px;
        }
        QTableView::item {
            background-color: #d3d2d0;
        }
        QTableView::item:selected {
            background-color: #b5cbd8;
            color: #000000;
        }
        QPushButton {
            background-color: rgb(193, 193, 193);
            border-radius: 20px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: rgb(170, 170, 170);
        }
        QPushButton:pressed {
            background-color: rgb(150, 150, 150);
        }
    )");

    portfolioTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    virtualPortfolioTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    portfolioTableView->verticalHeader()->setVisible(false);
    virtualPortfolioTableView->verticalHeader()->setVisible(false);

    token = getenv("TOKEN");
    client = new InvestApiClient("sandbox-invest-public-api.tinkoff.ru:443", token.toStdString()); // sandbox-

    auto accountService = std::dynamic_pointer_cast<Users>(client->service("users"));

    if (!accountService)
    {
        qDebug() << "Failed to get Users service";
        return;
    }

    auto accountListOfID = accountService->GetAccounts();
    auto accountID1 = dynamic_cast<GetAccountsResponse *>(accountListOfID.ptr().get());

    if (!accountID1)
    {
        qDebug() << "Failed to get accounts list";
        return;
    }

    for (int i = 0; i < accountID1->accounts_size(); ++i)
    {
        auto account = accountID1->accounts(i);
        accountIds.push_back(account.id().c_str());
        accountComboBox->addItem(account.name().c_str(), QVariant(account.id().c_str()));
    }

    connect(accountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAccountChanged(int)));
    connect(updateButton, &QPushButton::clicked, this, &Portfolio::onUpdateButtonClicked);

    if (!accountIds.empty())
    {
        updateBalance(accountIds[0]);
    }

    portfolioTableView->setModel(portfolioModel);
    virtualPortfolioTableView->setModel(virtualPortfolioModel);
}

void Portfolio::updateBalance(const QString &accountId)
{

    if (!client)
    {
        qDebug() << "InvestApiClient is not initialized";
        return;
    }

    auto operationService = std::dynamic_pointer_cast<Operations>(client->service("operations"));

    if (!operationService)
    {
        qDebug() << "Failed to get Operations service";
        return;
    }

    auto instrumentsService = std::dynamic_pointer_cast<Instruments>(client->service("instruments"));

    if (!instrumentsService)
    {
        qDebug() << "Failed to get Instruments service";
        return;
    }

    auto portfolioRequest = operationService->GetPortfolio(accountId.toStdString(), PortfolioRequest_CurrencyRequest::PortfolioRequest_CurrencyRequest_RUB);
    auto portfolioAns = dynamic_cast<PortfolioResponse *>(portfolioRequest.ptr().get());

    if (!portfolioAns)
    {
        qDebug() << "Failed to get portfolio response";
        return;
    }

    auto portfolioValue = portfolioAns->total_amount_portfolio().units();

    balanceLabel->setText("Balance: " + QString::number(portfolioValue) + " " + QString::fromStdString(portfolioAns->total_amount_portfolio().currency()));

    if (portfolioAns->positions().size() == 0)
    {
        portfolioModel->setRowCount(portfolioAns->positions().size());
    }
    else
    {

        // std::cout << portfolioAns->positions().size() << '\n';
        // auto portfolio1 = portfolioAns->positions(0);

        // std::cout << portfolioAns->positions(0) << '\n';

        for (int i = 0; i < portfolioAns->positions_size(); ++i)
        {
            const auto &position = portfolioAns->positions(i);

            // std::cout << position.figi() << '\t' << i << '\n';

            auto instrumentUID = position.instrument_uid();

            // if (position.instrument_type() == "currency"){
            //     auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
            //     auto instrumentResponse = dynamic_cast<CurrencyResponse*>(instrumentRequest.ptr().get());
            // }

            auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
            auto instrumentResponse = dynamic_cast<InstrumentResponse *>(instrumentRequest.ptr().get());

            // auto instrumentRequest = instrumentsService->ShareBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
            // auto shareResponse = dynamic_cast<ShareResponse*>(instrumentRequest.ptr().get());

            if (!instrumentResponse)
            {
                qDebug() << "Failed to get instrumentResponse";
                return;
            }

            // std::cout << "Я тут!" << '\n';

            auto instrumentInfo = instrumentResponse->instrument();
            auto instrumentName = instrumentInfo.name();
            auto instrumentTicker = instrumentInfo.ticker();

            // auto positionQuantity = position.quantity();
            // auto shareInfo = shareResponse->instrument();
            // auto shareName = shareInfo.name();
            // auto shareTicker = shareInfo.ticker();

            QList<QStandardItem *> rowItems;
            rowItems.append(new QStandardItem(QString::fromStdString(instrumentTicker)));
            rowItems.append(new QStandardItem(QString::fromStdString(instrumentName)));
            rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
            rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
            rowItems.append(new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency())));
            portfolioModel->appendRow(rowItems);
        }
    }

    auto virtualPositions = portfolioAns->virtual_positions();

    if (virtualPositions.size() == 0)
    {
        virtualPortfolioModel->setRowCount(virtualPositions.size());
    }
    else
    {

        // virtualPortfolioModel->setRowCount(virtualPositions.size() - 1);

        for (int i = 0; i < virtualPositions.size(); ++i)
        {
            const auto &position = virtualPositions[i];

            auto instrumentUID = position.instrument_uid();

            auto instrumentRequest = instrumentsService->GetInstrumentBy(tinkoff::public_::invest::api::contract::v1::InstrumentIdType::INSTRUMENT_ID_TYPE_UID, "", instrumentUID);
            auto instrumentResponse = dynamic_cast<InstrumentResponse *>(instrumentRequest.ptr().get());

            if (!instrumentResponse)
            {
                qDebug() << "Failed to get instrumentResponse";
                return;
            }

            auto instrumentInfo = instrumentResponse->instrument();
            auto instrumentName = instrumentInfo.name();
            auto instrumentTicker = instrumentInfo.ticker();

            QList<QStandardItem *> rowItems;
            rowItems.append(new QStandardItem(QString::fromStdString(instrumentTicker)));
            rowItems.append(new QStandardItem(QString::fromStdString(instrumentName)));
            rowItems.append(new QStandardItem(QString::number(position.quantity().units())));
            rowItems.append(new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency())));
            rowItems.append(new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency())));
            virtualPortfolioModel->appendRow(rowItems);
        }
    }

    auto yield = portfolioAns->expected_yield();

    double yieldValue = yield.units() + yield.nano() * 1e-9;
    yieldLabel->setText("Expected Yield: " + QString::number(yieldValue) + "%");
}

void Portfolio::onAccountChanged(int index)
{
    QString accountId = accountComboBox->itemData(index).toString();
    updateBalance(accountId);
}

void Portfolio::onUpdateButtonClicked()
{
    int currentIndex = accountComboBox->currentIndex();
    if (currentIndex >= 0)
    {
        QString accountId = accountComboBox->itemData(currentIndex).toString();
        portfolioModel->setRowCount(0);
        virtualPortfolioModel->setRowCount(0);
        updateBalance(accountId);
    }
}
