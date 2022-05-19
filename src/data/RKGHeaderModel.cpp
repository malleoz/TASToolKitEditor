#include "RKGHeaderModel.h"

#include <QBrush>


const QStringList RKGHeaderModel::EDITABLE_HEADER_NAMES =
{
    "Total Time", "Track", "Vehicle", "Character", "Date", "Controller",
    "Ghost Type", "Drift Type", "Lap Count", "Lap Time",
    "Country ID", "State ID", "Location ID", "Free Space", "Mii"
};


RKGHeaderModel::RKGHeaderModel(QObject *parent)
    : QAbstractTableModel{parent}
    , m_header()
{
    assert(EDITABLE_HEADER_NAMES.size() == EDITABLE_HEADER_COUNT);
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
        {
            if (index.column() == 0)
                return EDITABLE_HEADER_NAMES[index.row()];
            else
                return dataFromHeader(index.row());
        }
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::BackgroundRole:
            return index.column() == 0 ? QBrush(Qt::lightGray) : QVariant();
    }

    return QVariant();
}

QVariant RKGHeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0: return QString("HeadType");
            case 1: return QString("HeadData");
        }
    }

    return QVariant();
}


QString RKGHeaderModel::dataFromHeader(const int index) const
{
    switch(index)
    {
        case 0: return m_header.totalTime;
        case 1: return RKGInterpreter::trackDesc(m_header.trackId);
        case 2: return RKGInterpreter::vehicleDesc(m_header.vehicleID);
        case 3: return RKGInterpreter::characterDesc(m_header.characterID);
        case 4: return QString("%1.%2.%3").arg(QString::number(m_header.year + 2000),
                                               QString::number(m_header.month),
                                               QString::number(m_header.day));
        case 5: return RKGInterpreter::controllerDesc(m_header.controllerID);
        case 6: return RKGInterpreter::ghostTypeDesc(m_header.ghostType);
        case 7: return RKGInterpreter::driftDesc(m_header.driftType);
        case 8: return QString::number(m_header.lapCount);
        case 9: return m_header.lapTimes[0];
        case 10: return QString::number(m_header.countryID);
        case 11: return QString::number(m_header.stateID);
        case 12: return QString::number(m_header.locationID);
        case 13: return QString::number(m_header.freeSpace);
        case 14: return QString::number(m_header.miiCrc);

        default: return "";
    }
}
