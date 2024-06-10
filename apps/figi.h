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
#include <QTextEdit>
#include <QKeyEvent>

#include "investapiclient.h"
#include "marketdataservice.h"
#include "usersservice.h"
#include "ordersservice.h"
#include "instrumentsservice.h"
#include "operationsservice.h"
#include "mainwindow.h" 

class MainWindow;

class SearchTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit SearchTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

signals:
    void enterPressed();

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return or event->key() == Qt::Key_Enter) {
            emit enterPressed();
        } else {
            QTextEdit::keyPressEvent(event);
        }
    }
};

class DatabaseFigi : public QWidget {
    Q_OBJECT

public:
    explicit DatabaseFigi(QWidget *parent = nullptr);
    void insertSharesIntoDatabase(QString token);

private slots:
    void onSearchButtonClicked();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    QStandardItem* createNonEditableItem(const QString& text);
    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QLabel *enterLabel;
    SearchTextEdit *textEdit;
    QPushButton *searchButton;
    QLabel *suggestionsLabel;
    QTableView *tableView;
    QStandardItemModel *tableModel;
    QLabel *numElementsLabel;
    QSqlDatabase db;
    QString token;
    MainWindow *mainWindow;

    void initializeUI();
    void initializeDatabase();
    void setupConnections();
    void loadAllShares();
};

class ClickCounter : public QObject {
    Q_OBJECT

public:
    explicit ClickCounter(QObject *parent = nullptr);
    static void installOn(QWidget *widget);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    static size_t clickCount;
    void incrementClickCount();
};

#endif // __FIGI_H__
