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
                    return headName.append(": %1").arg(m_currentLapCount + 1);
                }

                return headName;
            }
            else
                return dataFromHeader(index);
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

bool RKGHeaderModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!checkIndex(index))
        return false;

    switch (role)
    {
        case Qt::EditRole:
        {
            if (index.column() == 1)
                return dataToHeader(index, value.toString());
            if (index.row() == LAPTIME_HEADER_INDEX)
            {
                QStringList list = value.toString().split(':');
                if (list.size() != 2)
                    return false;

                const int cLC = list[1].toInt();

                m_currentLapCount = cLC < RKGHeader::totalLapTimes ? cLC : RKGHeader::totalLapTimes - 1;
            }
            else
                return false;

            break;
        }

        default: return false;
    }

    return true;
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

bool RKGHeaderModel::dataToHeader(const QModelIndex& index, const QString& data)
{
    switch(index.row())
    {
        case 0:
        {
            m_header.totalTime = data;
            break;
        }
        case 1:
        {
            for (int tid = 0x0; tid < RKGHeader::trackAmount; tid++)
            {
                const TrackID trackId = static_cast<TrackID>(tid);

                if (data == RKGInterpreter::trackDesc(trackId))
                {
                    m_header.trackId = trackId;
                    return true;
                }
            }
            return false;
        }
        case 2:
        {
            for (int vid = 0x0; vid <= static_cast<int>(VehicleID::le_bike); vid++)
            {
                const VehicleID vehicleId = static_cast<VehicleID>(vid);

                if (data == RKGInterpreter::vehicleDesc(vehicleId))
                {
                    m_header.vehicleID = vehicleId;
                    return true;
                }
            }
            return false;
        }
        case 3:
        {
            for (int cid = 0x0; cid <= static_cast<int>(CharacterID::rs_menu); cid++)
            {
                const CharacterID characterId = static_cast<CharacterID>(cid);

                if (data == RKGInterpreter::characterDesc(characterId))
                {
                    m_header.characterID = characterId;
                    return true;
                }
            }
            return false;
        }
        case 4:
        {
            QStringList list = data.split('.');
            if (list.size() != 3)
                return false;

            m_header.year = (list[0].toUInt() - 2000) & 0x7F;
            m_header.month = list[1].toUInt() & 0xF;
            m_header.day = list[2].toUInt() & 0x1F;

            break;
        }
        case 5:
        {
            for (int cid = 0x0; cid <= static_cast<int>(ControllerType::GCN); cid++)
            {
                const ControllerType controllerId = static_cast<ControllerType>(cid);

                if (data == RKGInterpreter::controllerDesc(controllerId))
                {
                    m_header.controllerID = controllerId;
                    return true;
                }
            }
            return false;
        }
        case 6:
        {
            for (uint8_t gid = 0x1; gid <= 0x26; gid++)
            {
                if (data == RKGInterpreter::ghostTypeDesc(gid))
                {
                    m_header.ghostType = gid;
                    return true;
                }
            }
            return false;
        }
        case 7:
        {
            if (data == RKGInterpreter::driftDesc(DriftType::Manual))
            {
                m_header.driftType = DriftType::Manual;
                return true;
            }
            if (data == RKGInterpreter::driftDesc(DriftType::Automatic))
            {
                m_header.driftType = DriftType::Automatic;
                return true;
            }
            return false;
        }
        case 8:
        {
            const uint8_t lapCount = static_cast<uint8_t>(data.toUShort());
            m_header.lapCount = lapCount < RKGHeader::totalLapTimes ? lapCount : RKGHeader::totalLapTimes - 1;

            break;
        }
        case 9:
        {
            m_header.lapTimes[m_currentLapCount] = data;
            break;
        }
        case 10:
        {
            m_header.countryID = static_cast<uint8_t>(data.toUShort());
            break;
        }
        case 11:
        {
            m_header.stateID = static_cast<uint8_t>(data.toUShort());
            break;
        }
        case 12:
        {
            m_header.locationID = data.toUShort();
            break;
        }
        case 13:
        {
            m_header.freeSpace = data.toULong();
            break;
        }
        case 14:

        default: return false;
    }

    return true;
}
