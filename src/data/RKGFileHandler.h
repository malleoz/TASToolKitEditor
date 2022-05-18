#pragma once


#include <cstdint>
#include <QString>

#include "include/Definitions.h"
#include "include/RKGDefinitions.h"


class RKGFileHandler
{

public:
    RKGFileHandler();

    FileStatus loadRKGFile(const QString& path, RKGHeader& o_header, TTKFileData& o_fileData);
    void saveRKGFile(const QString& path);

private:
    void loadHeader(QDataStream& stream, RKGHeader& o_header);
    void loadInputs(QDataStream& stream, RKGHeader& o_header, TTKFileData& o_fileData);

    bool uncompressInputs(QDataStream& stream, QByteArray& o_uncompressedData, const RKGHeader& header);
    void compressInputs();

private:
    void decodeFaceButton(const uint8_t data, FrameData& row);
    void decodeDirectionInput(const uint8_t data, FrameData& row);
    QString decodeTrickInput(const uint16_t data, uint16_t& size);

    void encodeFaceButton();
    void encodeDirectionInput();
    void encodeTrickInput();

    void loadTail(QDataStream& stream, RKGHeader& o_header);
};
