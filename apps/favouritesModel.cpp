#include "favouritesModel.h"

FavouritesModel::FavouritesModel(QObject* parent, const QString& token)
    : QAbstractTableModel(parent)
    , token(token)
{
    std::vector<ShareInfo> figies = parseFavFigi(token);
    for (ShareInfo share: figies)
    {
        QList<QVariant> shareDescription({QString::fromStdString(share.name), QString::fromStdString(share.figi), QString::fromStdString(share.trading_status)});
        _sharesData.append(shareDescription);
    }

    _header.append({"Name", "FIGI", "Trading Status"});
}

int FavouritesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _sharesData.count();
}

int FavouritesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _header.count();
}

QVariant FavouritesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        try
        {
            return _sharesData.at(index.row()).at(index.column());
        } catch (...)
        {}
    }

    return QVariant();
}


QVariant FavouritesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
            return _header.at(section);
        else if (orientation == Qt::Vertical)
            return _sharesData.at(section).at(0);
    }
    return QVariant();
}