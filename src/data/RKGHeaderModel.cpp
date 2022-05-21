#include "RKGHeaderModel.h"

#include <QBrush>


const QStringList RKGHeaderModel::EDITABLE_HEADER_NAMES =
{
    "Total Time", "Track", "Vehicle", "Character", "Date", "Controller",
    "Ghost Type", "Drift Type", "Lap Count", "Lap :%1",
    "Country ID", "State ID", "Location ID", "Free Space", "Mii"
};


RKGHeaderModel::RKGHeaderModel(QObject *parent)
    : QAbstractTableModel{parent}
    , m_header()
    , m_currentLapCount(0)
{
    assert(EDITABLE_HEADER_NAMES.size() == EDITABLE_HEADER_COUNT);
}


Qt::ItemFlags RKGHeaderModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.column() == 1 || index.row() == LAPTIME_HEADER_INDEX)
        flags |= Qt::ItemIsEditable;


    return flags;
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
            {
                QString headName = EDITABLE_HEADER_NAMES[index.row()];

                if (index.row() == LAPTIME_HEADER_INDEX)
                {
                    return headName.arg(m_currentLapCount + 1);
                }

                return headName;
            }
            else
                return dataFromHeader(index);
        }
        case Qt::TextAlignmentRole:
            return Qt::AlignLeft + Qt::AlignVCenter;
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

bool RKGHeaderModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!checkIndex(index))
        return false;

    switch (role)
    {
        case Qt::EditRole:
        {
            if (index.column() == 1)
                return dataToHeader(index, value);
            if (index.row() == LAPTIME_HEADER_INDEX)
                m_currentLapCount = value.toInt();
            else
                return false;

            break;
        }

        default: return false;
    }

    return true;
}

void RKGHeaderModel::setHeader(const RKGHeader& header)
{
    beginResetModel();
    m_header = header;
    endResetModel();
}


QString RKGHeaderModel::dataFromHeader(const QModelIndex& index) const
{
    switch(index.row())
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
        case 9: return m_header.lapTimes[m_currentLapCount];
        case 10: return QString::number(m_header.countryID);
        case 11: return QString::number(m_header.stateID);
        case 12: return QString::number(m_header.locationID);
        case 13: return QString::number(m_header.freeSpace);
        case 14: return QString::number(m_header.miiCrc);

        default: return "";
    }
}

bool RKGHeaderModel::dataToHeader(const QModelIndex& index, const QVariant& data)
{
    switch(index.row())
    {
        case 0:
        {
            m_header.totalTime = data.toString();
            break;
        }
        case 1:
        {
            const int trackIndex = data.toInt();
            const TrackID trackId = RKGInterpreter::TRACK_ID_ORDERED_LIST[trackIndex];

            m_header.trackId = trackId;
            break;
        }
        case 2:
        {
            m_header.vehicleID = static_cast<VehicleID>(data.toInt());
            break;
        }
        case 3:
        {
            m_header.characterID = static_cast<CharacterID>(data.toInt());
            break;
        }
        case 4:
        {
            QStringList list = data.toString().split('.');
            if (list.size() != 3)
                return false;

            m_header.year = (list[0].toUInt() - 2000) & 0x7F;
            m_header.month = list[1].toUInt() & 0xF;
            m_header.day = list[2].toUInt() & 0x1F;

            break;
        }
        case 5:
        {
            m_header.controllerID = static_cast<ControllerType>(data.toInt());
            break;
        }
        case 6:
        {
            m_header.ghostType = static_cast<uint8_t>(data.toInt() - 1);
            break;
        }
        case 7:
        {
            m_header.driftType = static_cast<DriftType>(data.toInt());
            break;
        }
        case 8:
        {
            const uint8_t lapCount = static_cast<uint8_t>(data.toString().toUShort());
            m_header.lapCount = lapCount < RKGHeader::TOTAL_LAP_TIMES ? lapCount : RKGHeader::TOTAL_LAP_TIMES - 1;

            break;
        }
        case 9:
        {
            m_header.lapTimes[m_currentLapCount] = data.toString();
            break;
        }
        case 10:
        {
            m_header.countryID = static_cast<uint8_t>(data.toString().toUShort());
            break;
        }
        case 11:
        {
            m_header.stateID = static_cast<uint8_t>(data.toString().toUShort());
            break;
        }
        case 12:
        {
            m_header.locationID = data.toString().toUShort();
            break;
        }
        case 13:
        {
            m_header.freeSpace = data.toUInt();
            break;
        }
        case 14:

        default: return false;
    }

    return true;
}
