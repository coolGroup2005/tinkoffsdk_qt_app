#ifndef __FAVOURITES_H__
#define __FAVOURITES_H__

#include "homepage/homepage.h"
#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QVariant>


class FavouritesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FavouritesModel(QObject* parent = nullptr);

protected:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QList<QList<QVariant>> _sharesData;
    QList<QString> _header;
};

#endif// __FAVOURITES_H__