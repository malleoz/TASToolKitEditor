#include "RKGFileHandler.h"

#include <QFile>
#include <QMessageBox>

#include <QDataStream>
#include <QByteArray>

RKGFileHandler::RKGFileHandler()
{
}

FileStatus RKGFileHandler::loadRKGFile(const QString& path, TTKFileData& o_fileData)
{
    QFile fp(path);

    if (!fp.open(QIODevice::ReadOnly))
    {
        const QString errorTitle = "Error Opening File";
        const QString errorMsg = "This program does not have sufficient permissions to modify the file.\n\n" \
            "Try running this program in administrator mode and make sure the file is not open in another program.";
        QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg, QMessageBox::StandardButton::Ok);

        fp.close();
        return FileStatus::InsufficientWritePermission;
    }

    QDataStream in(&fp);
    in.setByteOrder(QDataStream::BigEndian);

    uint32_t dataMass32 = 0;
    in >> dataMass32;

    in >> dataMass32;
    m_rkgHeader.totalTime.minutes = dataMass32 >> 25 & 0x7F;
    m_rkgHeader.totalTime.seconds = dataMass32 >> 18 & 0x7F;
    m_rkgHeader.totalTime.milliseconds = dataMass32 >> 8 & 0x3FF;
    m_rkgHeader.trackId = dataMass32 >> 2 & 0x3F;

    in >> dataMass32;
    m_rkgHeader.vehicleID = dataMass32 >> 26 & 0x3F;
    m_rkgHeader.characterID = dataMass32 >> 20 & 0x3F;
    m_rkgHeader.year = dataMass32 >> 13 & 0x7F;
    m_rkgHeader.month = dataMass32 >> 9 & 0xF;
    m_rkgHeader.day = dataMass32 >> 4 & 0x1F;
    m_rkgHeader.controllerId = dataMass32 & 0xF;

    uint16_t dataMass16 = 0;

    in >> dataMass16;
    m_rkgHeader.isCompressed = dataMass16 >> 11 & 0x1;
    m_rkgHeader.ghostType = dataMass16 >> 2 & 0x7F;
    m_rkgHeader.driftType = dataMass16 >> 1 & 0x1;

    in >> m_rkgHeader.inputDataLength;
    in >> m_rkgHeader.lapCount;

    uint8_t lapSplits[totalLapTimes * lapSplitSize];
    in.readRawData(reinterpret_cast<char*>(lapSplits), sizeof(lapSplits));

    for (int i = 0; i < totalLapTimes; i++)
    {
        m_rkgHeader.lapTimes[i].from3Byte(&(lapSplits[i*lapSplitSize]));
    }

    in >> dataMass16;

    in >> m_rkgHeader.countryId;
    in >> m_rkgHeader.stateId;
    in >> m_rkgHeader.locationId;

    in >> m_rkgHeader.freeSpace;

    in.readRawData(reinterpret_cast<char*>(m_rkgHeader.mii), sizeof(m_rkgHeader.mii));
    in >> m_rkgHeader.miiCrc;

    return FileStatus::Success;
}

void RKGFileHandler::saveRKGFile(const QString& path)
{
}
