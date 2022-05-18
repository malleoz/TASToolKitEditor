#pragma once


#include <cstdint>
#include <QString>

#include "include/Definitions.h"
#include "include/RKGDefinitions.h"


class RKGFileHandler
{
private:
    RKGFileHandler() {}

public:
    static FileStatus loadRKGFile(const QString& path, RKGHeader& o_header, TTKFileData& o_fileData);
    static FileStatus saveRKGFile(const QString& path);

private:
    static void loadHeader(QDataStream& stream, RKGHeader& o_header);
    static void loadInputs(QDataStream& stream, RKGHeader& o_header, TTKFileData& o_fileData);

    static bool uncompressInputs(QDataStream& stream, QByteArray& o_uncompressedData, const RKGHeader& header);
    static void compressInputs();

private:
    static void decodeFaceButton(const uint8_t data, FrameData& row);
    static void decodeDirectionInput(const uint8_t data, FrameData& row);
    static QString decodeTrickInput(const uint16_t data, uint16_t& size);

    static void encodeFaceButton();
    static void encodeDirectionInput();
    static void encodeTrickInput();

    static void loadTail(QDataStream& stream, RKGHeader& o_header);

};
