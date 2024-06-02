#include "portfolio.h"
#include <QVBoxLayout>
#include <cstdlib>
#include <QDebug> // for finding segmentation fault
#include <QHeaderView>
#include "mainwindow.h" 


Portfolio::Portfolio(QWidget *parent, const QString& token) : QWidget(parent), token(token)
{
    balanceLabel = new QLabel(this);
    yieldLabel = new QLabel(this);
    userInfoLabel = new QLabel(this);
    // balanceLabel->setStyleSheet("font-weight: bold; font-size: 20px; ");
    // yieldLabel->setStyleSheet("font-weight: bold; font-size: 20px; ");
    // userInfoLabel->setStyleSheet("font-weight: bold; font-size: 20px; ");
    accountComboBox = new QComboBox(this);
    portfolioTableView = new QTableView(this);
    portfolioModel = new QStandardItemModel(this);
    portfolioModel->setHorizontalHeaderLabels({"FIGI", "Ticker", "Name", "Quantity", "Current Price", "Value"});

    virtualPortfolioTableView = new QTableView(this);
    virtualPortfolioModel = new QStandardItemModel(this);
    virtualPortfolioModel->setHorizontalHeaderLabels({"FIGI", "Ticker", "Name", "Quantity", "Current Price", "Value"});

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
        "}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accountComboBox);
    layout->addWidget(balanceLabel);
    layout->addWidget(yieldLabel);
    layout->addWidget(userInfoLabel);
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
        auto fillIn = account.name() + ", ID: " + account.id();
        accountComboBox->addItem(fillIn.c_str(), QVariant(account.id().c_str()));
    }

    
    connect(portfolioTableView, &QTableView::doubleClicked, this, &Portfolio::onTableDoubleClicked);
    connect(virtualPortfolioTableView, &QTableView::doubleClicked, this, &Portfolio::onVirtualTableDoubleClicked);
    connect(accountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAccountChanged(int)));
    connect(updateButton, &QPushButton::clicked, this, &Portfolio::onUpdateButtonClicked);

    if (!accountIds.empty())
    {
        updateBalance(accountIds[0]);
        updateUserInfo();
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
        for (int i = 0; i < portfolioAns->positions_size(); ++i)
        {
            const auto &position = portfolioAns->positions(i);

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
            QStandardItem *figiItem = new QStandardItem(QString::fromStdString(instrumentInfo.figi()));
            QStandardItem *tickerItem = new QStandardItem(QString::fromStdString(instrumentTicker));
            QStandardItem *nameItem = new QStandardItem(QString::fromStdString(instrumentName));
            QStandardItem *quantityItem = new QStandardItem(QString::number(position.quantity().units()));
            QStandardItem *priceItem = new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency()));
            QStandardItem *valueItem = new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency()));

            figiItem->setFlags(figiItem->flags() & ~Qt::ItemIsEditable);
            tickerItem->setFlags(tickerItem->flags() & ~Qt::ItemIsEditable);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            quantityItem->setFlags(quantityItem->flags() & ~Qt::ItemIsEditable);
            priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);

            rowItems.append(figiItem);
            rowItems.append(tickerItem);
            rowItems.append(nameItem);
            rowItems.append(quantityItem);
            rowItems.append(priceItem);
            rowItems.append(valueItem);

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
            QStandardItem *figiItem = new QStandardItem(QString::fromStdString(instrumentInfo.figi()));
            QStandardItem *ticker = new QStandardItem(QString::fromStdString(instrumentTicker));
            QStandardItem *nameItem = new QStandardItem(QString::fromStdString(instrumentName));
            QStandardItem *quantityItem = new QStandardItem(QString::number(position.quantity().units()));
            QStandardItem *priceItem = new QStandardItem(QString::number(position.current_price().units()) + " " + QString::fromStdString(position.current_price().currency()));
            QStandardItem *valueItem = new QStandardItem(QString::number(position.current_price().units() * position.quantity().units()) + " " + QString::fromStdString(position.current_price().currency()));
            
            figiItem->setFlags(figiItem->flags() & ~Qt::ItemIsEditable);
            ticker->setFlags(ticker->flags() & ~Qt::ItemIsEditable);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            quantityItem->setFlags(quantityItem->flags() & ~Qt::ItemIsEditable);
            priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);

            rowItems.append(figiItem);
            rowItems.append(ticker);
            rowItems.append(nameItem);
            rowItems.append(quantityItem);
            rowItems.append(priceItem);
            rowItems.append(valueItem);

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

void Portfolio::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        qDebug() << "Invalid index!";
        return;
    }

    int row = index.row();
    QString ticker = portfolioModel->item(row, 0)->text();
    QString name = portfolioModel->item(row, 1)->text();

    // std::cout << "13213213123" << '\n';

    std::string tickerStd = ticker.toStdString();
    std::string nameStd = name.toStdString();

    // тут вызвать функцию показа свечей + добавить в хедер инклюд
    mainWindow->openShares(tickerStd, nameStd);
}

void Portfolio::onVirtualTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        qDebug() << "Invalid index!";
        return;
    }

    int row = index.row();
    QStandardItem* nameItem = virtualPortfolioModel->item(row, 1);

    if (!nameItem) {
        qDebug() << "Invalid name item at row:" << row;
        return;
    }

    QStandardItem* tickerItem = virtualPortfolioModel->item(row, 0);

    if (!tickerItem) {
        qDebug() << "Invalid ticker item at row:" << row;
        return;
    }

    QString name = nameItem->text();
    QString ticker = tickerItem->text();

    // qDebug() << "Double clicked on virtual table row:" << row << "Name:" << name << "Ticker:" << ticker;

    // std::cout << "виртуалка" << '\n';

    // тут вызвать функцию показа свечей + добавить в хедер инклюд
    // showCandlestickChart(ticker, name);
    std::string tickerStd = ticker.toStdString();
    std::string nameStd = name.toStdString();
    mainWindow->openShares(tickerStd, nameStd);
}

void Portfolio::updateUserInfo()
{
    auto userService = std::dynamic_pointer_cast<Users>(client->service("users"));

    if (!userService) {
        qDebug() << "Failed to get Users service";
        return;
    }

    auto userInfoRequest = userService->GetInfo();
    auto userInfoResponse = dynamic_cast<GetInfoResponse*>(userInfoRequest.ptr().get());

    if (!userInfoResponse) {
        qDebug() << "Failed to get user info response";
        return;
    }

    QString userInfoText;
    userInfoText += "Premium Status: " + QString(userInfoResponse->prem_status() ? "Yes" : "No") + "\n" + "\n";
    userInfoText += "Qualified Investor: " + QString(userInfoResponse->qual_status() ? "Yes" : "No") + "\n"  + "\n";
    userInfoText += "Tariff: " + QString::fromStdString(userInfoResponse->tariff()) + "\n";

    userInfoLabel->setText(userInfoText);
}