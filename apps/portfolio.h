#ifndef PORTFOLIOBALANCE_H
#define PORTFOLIOBALANCE_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include "investapiclient.h"
#include "usersservice.h"
#include "operationsservice.h"
#include "sandboxservice.h"
#include <ordersservice.h> // for adding shares into positions
#include <instrumentsservice.h> // for getting Tiker

class MainWindow; 

class Portfolio : public QWidget
{
    Q_OBJECT

public:
    explicit Portfolio(QWidget *parent = nullptr, const QString& token = "");
    void updateBalance(const QString& accountId);
    void updateUserInfo();

private slots:
    void onAccountChanged(int index);
    void onUpdateButtonClicked();
    void onTableDoubleClicked(const QModelIndex &index);
    void onVirtualTableDoubleClicked(const QModelIndex &index);

private:
    void updateExpectedYield(const Quotation& yield);
    void updateVirtualPositions(const google::protobuf::RepeatedPtrField<VirtualPortfolioPosition>& virtualPositions);

    QLabel *balanceLabel;
    QLabel *yieldLabel;
    QLabel *userInfoLabel;
    InvestApiClient *client;
    QString token;
    QComboBox *accountComboBox;
    std::vector<QString> accountIds;
    QTableView *portfolioTableView;
    QStandardItemModel *portfolioModel;
    QTableView *virtualPortfolioTableView;
    QStandardItemModel *virtualPortfolioModel;
    QPushButton *updateButton;
    MainWindow *mainWindow;
};

#endif // PORTFOLIOBALANCE_H

