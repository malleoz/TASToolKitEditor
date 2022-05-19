#include "RKGFileHandler.h"

#include <QFile>
#include <QMessageBox>

#include <QDataStream>
#include <QByteArray>


FileStatus RKGFileHandler::loadRKGFile(const QString& path, RKGHeader& o_header, TTKFileData& o_fileData)
{
    QFile fp(path);

    if (!fp.open(QIODevice::ReadOnly))
    {
        const QString errorTitle = "Error Opening File";
        const QString errorMsg = "This program was not able to the specified file.";
        QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg, QMessageBox::StandardButton::Ok);

        fp.close();
        return FileStatus::InsufficientWritePermission;
    }

    QDataStream in(&fp);
    in.setByteOrder(QDataStream::BigEndian);

    loadHeader(in, o_header);

    if (o_header.isCompressed)
    {
        QByteArray uncompressedData;
        uncompressInputs(in, uncompressedData, o_header);

        QDataStream in(uncompressedData);

        o_header.isCompressed = false;
        loadTail(in, o_header);

        loadInputs(in, o_header, o_fileData);
    }
    else
    {
        loadInputs(in, o_header, o_fileData);
    }

    fp.close();

    return FileStatus::Success;
}

FileStatus RKGFileHandler::saveRKGFile(const QString& path)
{
    return FileStatus::Success;
}


void RKGFileHandler::loadHeader(QDataStream& stream, RKGHeader& o_header)
{
    uint32_t dataMass32 = 0;
    stream.skipRawData(4);


    stream >> dataMass32;
    o_header.totalTime.minutes = dataMass32 >> 25 & 0x7F;
    o_header.totalTime.seconds = dataMass32 >> 18 & 0x7F;
    o_header.totalTime.milliseconds = dataMass32 >> 8 & 0x3FF;
    o_header.trackId = static_cast<TrackID>(dataMass32 >> 2 & 0x3F);

    stream >> dataMass32;
    o_header.vehicleID = static_cast<VehicleID>(dataMass32 >> 26 & 0x3F);
    o_header.characterID = static_cast<CharacterID>(dataMass32 >> 20 & 0x3F);
    o_header.year = dataMass32 >> 13 & 0x7F;
    o_header.month = dataMass32 >> 9 & 0xF;
    o_header.day = dataMass32 >> 4 & 0x1F;
    o_header.controllerID = static_cast<ControllerType>(dataMass32 & 0xF);

    uint16_t dataMass16 = 0;

    stream >> dataMass16;
    o_header.isCompressed = dataMass16 >> 11 & 0x1;
    o_header.ghostType = dataMass16 >> 2 & 0x7F;
    o_header.driftType = static_cast<DriftType>(dataMass16 >> 1 & 0x1);

    stream >> o_header.inputDataLength;
    stream >> o_header.lapCount;

    uint8_t lapSplits[RKGHeader::totalLapTimes * RKGHeader::lapSplitSize];
    stream.readRawData(reinterpret_cast<char*>(lapSplits), sizeof(lapSplits));

    for (int i = 0; i < RKGHeader::totalLapTimes; i++)
    {
        o_header.lapTimes[i].from3Byte(&(lapSplits[i*RKGHeader::lapSplitSize]));
    }

    stream.skipRawData(2);

    stream >> o_header.countryID;
    stream >> o_header.stateID;
    stream >> o_header.locationID;

    stream >> o_header.freeSpace;

    stream.readRawData(reinterpret_cast<char*>(o_header.mii), sizeof(o_header.mii));
    stream >> o_header.miiCrc;

    loadTail(stream, o_header);
}

void RKGFileHandler::loadInputs(QDataStream& stream, RKGHeader& o_header, TTKFileData& o_fileData)
{
    uint16_t theoreticalInputLength = o_header.tail.uT.faceButtonCount * 2 +
                                      o_header.tail.uT.directionInputCount * 2 +
                                      o_header.tail.uT.trickInputCount * 2 +
                                      sizeof(o_header.tail);

    if (o_header.inputDataLength != theoreticalInputLength)
    {
        o_header.inputDataLength = theoreticalInputLength;

        const QString errorTitle = "GAS Alert";
        const QString errorMsg = "Your ghost file is corrupted due to Ghost Always Saves code.\n" \
                                 "Don't worry, we'll fix that for you";
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

bool RKGFileHandler::uncompressInputs(QDataStream& stream, QByteArray& o_uncompressedData, const RKGHeader& header)
{
    int uncompYazSize;
    stream >> uncompYazSize;

    if (header.inputDataLength != uncompYazSize)
        return false;

    stream.skipRawData(8);

    uint8_t bitCount = 0;
    uint8_t bitFlags = 0;

    while (o_uncompressedData.size() < uncompYazSize)
    {
        // Get BitFlags
        if (bitCount == 0)
        {
            stream >> bitFlags;
            bitCount = 8;
        }

        bitCount--;

        // If MSB is true, straight copy
        if (bitFlags & 0x80)
        {
            int8_t d;
            stream >> d;
            o_uncompressedData.append(d);
        }
        // If MSB is false, use copy algorithm
        else
        {
            uint16_t d;
            stream >> d;

            // copySize is in X0 00 (...)
            uint16_t copyAmount = d >> 12 & 0xF;
            // negative offset is in 0X XX (...)
            const uint16_t offset = static_cast<uint16_t>(o_uncompressedData.size()) - (d & 0xFFF) - 1;

            // if copyAmount is 0, the amount is instead stored in 00 00 XX
            if (copyAmount == 0)
            {
                uint8_t d2;
                stream >> d2;
                copyAmount = d2 + 0x12;
            }
            else
            {
                copyAmount += 2;
            }

            // copy and append data from the specified offset in uncompressed data to the end
            for (uint16_t i = 0; i < copyAmount; i++)
            {
                o_uncompressedData.append(o_uncompressedData.at(offset + i));
            }
        }

        bitFlags <<= 1;
    }

    return true;
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

void RKGFileHandler::loadTail(QDataStream& stream, RKGHeader& o_header)
{
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
