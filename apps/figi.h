#ifndef __FIGI_H__
#define __FIGI_H__

#include <QObject>
#include <QWidget>
#include <QStringListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>


#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"

#include "instrumentsservice.h"
#include "operationsservice.h"

class DatabaseFigi : public QWidget {
    Q_OBJECT

public:
    explicit DatabaseFigi(QWidget *parent = nullptr);
    void insertSharesIntoDatabase();

private slots:
    void onSearchButtonClicked();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QLabel *enterLabel;
    QTextEdit *textEdit;
    QPushButton *searchButton;
    QLabel *suggestionsLabel;
    QTableView *tableView;
    QStandardItemModel *tableModel;
    QLabel *numElementsLabel;
    QSqlDatabase db;

    void initializeUI();
    void initializeDatabase();
    void setupConnections();
    void loadAllShares();
};

#endif // __FIGI_H__
