#include "RKGHeaderModel.h"

#include <QBrush>

RKGHeaderModel::RKGHeaderModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}


Qt::ItemFlags RKGHeaderModel::flags(const QModelIndex& index) const
{
    return QAbstractTableModel::flags(index);
}

int RKGHeaderModel::rowCount(const QModelIndex&) const
{
    return EDITABLE_HEADER_COUNT;
}

int RKGHeaderModel::columnCount(const QModelIndex&) const
{
    return 2;
}

QVariant RKGHeaderModel::data(const QModelIndex& index, int role) const
{

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::CheckStateRole:
            break;
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::BackgroundRole:
            return index.column() == 0 ? QBrush(Qt::gray) : QVariant();
    }

    return QVariant();
}
