#include "favouritesProxyModel.h"


ProxyModel::ProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}


bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex tradeStatusIndex = sourceModel()->index(source_row, 2, source_parent);

    if (sourceModel()->data(tradeStatusIndex) != _trading_status)
        return false;

    return true;
}

QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

void ProxyModel::setTradingStatus(const QString& tradeStatus)
{
    if (_trading_status != tradeStatus)
        _trading_status = tradeStatus;
    invalidateFilter();
}