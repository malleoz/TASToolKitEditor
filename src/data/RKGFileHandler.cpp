#include "RKGFileHandler.h"

#include <QFile>
#include <QMessageBox>

#include <QDataStream>
#include <QByteArray>

RKGFileHandler::RKGFileHandler()
{
}

FileStatus RKGFileHandler::loadRKGFile(const QString& path, RKGHeader& o_header, TTKFileData& o_fileData)
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

    loadHeader(in, o_header);

    if (o_header.isCompressed)
    {
        uncompressInputs(in, o_header);
    }

    loadInputs(in, o_header, o_fileData);

    fp.close();

    return FileStatus::Success;
}

void RKGFileHandler::saveRKGFile(const QString& path)
{
}


void RKGFileHandler::loadHeader(QDataStream& stream, RKGHeader& o_header)
{
    uint32_t dataMass32 = 0;
    stream >> dataMass32;

    stream >> dataMass32;
    o_header.totalTime.minutes = dataMass32 >> 25 & 0x7F;
    o_header.totalTime.seconds = dataMass32 >> 18 & 0x7F;
    o_header.totalTime.milliseconds = dataMass32 >> 8 & 0x3FF;
    o_header.trackId = dataMass32 >> 2 & 0x3F;

    stream >> dataMass32;
    o_header.vehicleID = dataMass32 >> 26 & 0x3F;
    o_header.characterID = dataMass32 >> 20 & 0x3F;
    o_header.year = dataMass32 >> 13 & 0x7F;
    o_header.month = dataMass32 >> 9 & 0xF;
    o_header.day = dataMass32 >> 4 & 0x1F;
    o_header.controllerId = dataMass32 & 0xF;

    uint16_t dataMass16 = 0;

    stream >> dataMass16;
    o_header.isCompressed = dataMass16 >> 11 & 0x1;
    o_header.ghostType = dataMass16 >> 2 & 0x7F;
    o_header.driftType = dataMass16 >> 1 & 0x1;

    stream >> o_header.inputDataLength;
    stream >> o_header.lapCount;

    uint8_t lapSplits[RKGHeader::totalLapTimes * RKGHeader::lapSplitSize];
    stream.readRawData(reinterpret_cast<char*>(lapSplits), sizeof(lapSplits));

    for (int i = 0; i < RKGHeader::totalLapTimes; i++)
    {
        o_header.lapTimes[i].from3Byte(&(lapSplits[i*RKGHeader::lapSplitSize]));
    }

    stream >> dataMass16;

    stream >> o_header.countryId;
    stream >> o_header.stateId;
    stream >> o_header.locationId;

    stream >> o_header.freeSpace;

    stream.readRawData(reinterpret_cast<char*>(o_header.mii), sizeof(o_header.mii));
    stream >> o_header.miiCrc;

    if (o_header.isCompressed)
    {
        stream >> o_header.tail.cT.dataLength;
        stream.readRawData(reinterpret_cast<char*>(o_header.tail.cT.YAZ1), sizeof(o_header.tail.cT.YAZ1));
    }
    else
    {
        stream >> o_header.tail.uT.faceButtonCount;
        stream >> o_header.tail.uT.directionInputCount;
        stream >> o_header.tail.uT.trickInputCount;
        stream >> o_header.tail.uT._padding;
    }
}

void RKGFileHandler::loadInputs(QDataStream& stream, RKGHeader& o_header, TTKFileData& o_fileData)
{

    if (o_header.inputDataLength != (o_header.tail.uT.faceButtonCount +
                                     o_header.tail.uT.directionInputCount +
                                     o_header.tail.uT.trickInputCount +
                                     sizeof(o_header.tail)))
    {
        o_header.inputDataLength = o_header.tail.uT.faceButtonCount +
                                   o_header.tail.uT.directionInputCount +
                                   o_header.tail.uT.trickInputCount +
                                   sizeof(o_header.tail);

        const QString errorTitle = "GAS Alert";
        const QString errorMsg = "Your ghost file is corrupted due to Ghost Always Saves code.\n" \
                                 "Don't worry, we'll fix that for you *insert smiley here*.";
        QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg);
    }

    for (int i = 0; i < o_header.tail.uT.faceButtonCount; i++)
    {
        uint8_t data = 0;
        uint8_t size = 0;

        stream >> data;
        stream >> size;

        FrameData newRow = {};
        decodeFaceButton(data, newRow);

        o_fileData.insert(o_fileData.length(), size, newRow);
    }

    int count = 0;

    for (int i = 0; i < o_header.tail.uT.directionInputCount; i++)
    {
        uint8_t data = 0;
        uint8_t size = 0;

        stream >> data;
        stream >> size;

        FrameData newRow = {};
        decodeDirectionInput(data, newRow);

        for (int j = 0; j < size; j++)
        {
            o_fileData[count++].append(newRow);
        }
    }

    count = 0;

    for (int i = 0; i < o_header.tail.uT.trickInputCount; i++)
    {
        uint16_t data = 0;
        uint16_t size = 0;
        stream >> data;

        const QString trickString = decodeTrickInput(data, size);

        for (int j = 0; j < size; j++)
        {
            o_fileData[count++].append(trickString);
        }
    }
}

void RKGFileHandler::uncompressInputs(QDataStream& stream, const RKGHeader& o_header)
{
    QByteArray newStream;


}


void RKGFileHandler::decodeFaceButton(const uint8_t data, FrameData& row)
{
    const QString buttonA = QString::number(data & 0x1);
    const QString buttonB = QString::number(data >> 1 & 0x1);
    const QString buttonL = QString::number(data >> 2 & 0x1);

    row.append(buttonA);
    row.append(buttonB);
    row.append(buttonL);
}

void RKGFileHandler::decodeDirectionInput(const uint8_t data, FrameData& row)
{
    const QString stickH = QString::number(data >> 4 & 0xF);
    const QString stickV = QString::number(data & 0xF);

    row.append(stickH);
    row.append(stickV);
}

QString RKGFileHandler::decodeTrickInput(const uint16_t data, uint16_t& size)
{
    const QString dPad = QString::number(data >> 12 & 0x7);
    size = data & 0xFFF;

    return dPad;
}
