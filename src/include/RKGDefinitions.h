#pragma once

#include <cstdint>

//enum class TrackID
//{
//    LC = 0x08,
//    MMM = 0x01,
//    MG = 0x02,
//    TF = 0x04,

//    MC = 0x00,
//    CM = 0x05,
//    DKS = 0x06,
//    WGM = 0x07,

//    DC = 0x09,
//    KC = 0x0F,
//    MT = 0x0B,
//    GV = 0x03,

//    DDR = 0x0E,
//    MH = 0x0A,
//    BC = 0x0C,
//    RR = 0x0D
//};

//enum class VehicleID
//{
//    S_St_Bike = 0xFF
//};

//enum class CharacterID
//{
//    Baby_Mario = 0xFF
//};

//enum class ControllerID
//{
//    Wheel = 0
//};

enum DriftType
{
    Manual = 0x0,
    Automatic = 0x1
};

struct RKGHeader
{
public:
    static const int totalLapTimes = 0xB;
    static const int lapSplitSize = 0x3;
    static const int miiDataSize = 0x4A;

    struct TimeSignature
    {
        uint8_t minutes;
        uint8_t seconds;
        uint16_t milliseconds;

        inline void from3Byte(const uint8_t* const byte3)
        {
            minutes = byte3[0] >> 1 & 0x7F;
            seconds = (byte3[0] << 6 & 0x40) + (byte3[1] >> 2 & 0x3F);
            milliseconds = (byte3[1] << 8 & 0x300) + (byte3[2] & 0xFF);
        }

        // ToDo: Test
        inline void to3Byte(uint8_t* const byte3)
        {
            byte3[0] = (minutes << 1 & 0xFE) + (seconds >> 6 & 0x1);
            byte3[1] = (seconds << 2 & 0xF7) + (milliseconds >> 8 & 0x3);
            byte3[2] = milliseconds & 0xFF;
        }
    };

    struct UncompTail
    {
        uint16_t faceButtonCount;
        uint16_t directionInputCount;
        uint16_t trickInputCount;
        uint16_t _padding;
    };

    struct CompTail
    {
        uint32_t dataLength;
        uint8_t YAZ1[4];
    };

    static_assert(sizeof(UncompTail) == sizeof(CompTail), "Struct not of equal size.");

public:
    const uint8_t RKGD[4] = {0x52, 0x4B, 0x47, 0x44};

    TimeSignature totalTime;

    uint8_t trackId;


    uint8_t vehicleID;
    uint8_t characterID;

    uint8_t year;
    uint8_t month;
    uint8_t day;

    uint8_t controllerId;


    bool isCompressed;

    uint8_t ghostType;
    uint8_t driftType;


    uint16_t inputDataLength;
    uint8_t lapCount;

    TimeSignature lapTimes[totalLapTimes];

    uint8_t countryId;
    uint8_t stateId;
    uint16_t locationId;

    uint32_t freeSpace;

    uint8_t mii[miiDataSize];
    uint16_t miiCrc;

    union {
        UncompTail uT;
        CompTail cT;
    } tail;

    static_assert(sizeof(tail) == 0x8, "Union consists of wrong data size.");
};
