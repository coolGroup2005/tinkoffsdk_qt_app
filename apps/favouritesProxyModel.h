#ifndef __PROXYMODEL_H__
#define __PROXYMODEL_H__

#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ProxyModel(QObject* parent = 0);
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

public slots:
    void setTradingStatus(const QString& tradeStatus);

private:
    QString _trading_status;
};

#endif// __PROXYMODEL_H__
