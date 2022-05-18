#pragma once

#include <cstdint>
#include <QString>

// https://wiki.tockdom.com/wiki/List_of_Identifiers

enum class TrackID
{
    LC = 0x08,
    MMM = 0x01,
    MG = 0x02,
    TF = 0x04,

    MC = 0x00,
    CM = 0x05,
    DKS = 0x06,
    WGM = 0x07,

    DC = 0x09,
    KC = 0x0F,
    MT = 0x0B,
    GV = 0x03,

    DDR = 0x0E,
    MH = 0x0A,
    BC = 0x0C,
    RR = 0x0D,

    rPB = 0x10,
    rYF = 0x14,
    rGV2 = 0x19,
    rMR = 0x1A,

    rSL = 0x1B,
    rSGB = 0x1F,
    rDS = 0x17,
    rWS = 0x12,

    rDH = 0x15,
    rBC3 = 0x1E,
    rDKJP = 0x1D,
    rMC = 0x11,

    rMC3 = 0x18,
    rPG = 0x16,
    rDKM = 0x13,
    rBC = 0x1C
};

enum class VehicleID
{
    sdf_kart = 0x00,
    mdf_kart = 0x01,
    ldf_kart = 0x02,
    sa_kart = 0x03,
    ma_kart = 0x04,
    la_kart = 0x05,
    sb_kart = 0x06,
    mb_kart = 0x07,
    lb_kart = 0x08,
    sc_kart = 0x09,
    mc_kart = 0x0A,
    lc_kart = 0x0B,
    sd_kart = 0x0C,
    md_kart = 0x0D,
    ld_kart = 0x0E,
    se_kart = 0x0F,
    me_kart = 0x10,
    le_kart = 0x11,
    sdf_bike = 0x12,
    mdf_bike = 0x13,
    ldf_bike = 0x14,
    sa_bike = 0x15,
    ma_bike = 0x16,
    la_bike = 0x17,
    sb_bike = 0x18,
    mb_bike = 0x19,
    lb_bike = 0x1A,
    sc_bike = 0x1B,
    mc_bike = 0x1C,
    lc_bike = 0x1D,
    sd_bike = 0x1E,
    md_bike = 0x1F,
    ld_bike = 0x20,
    se_bike = 0x21,
    me_bike = 0x22,
    le_bike = 0x23
};

enum class CharacterID
{
    mr = 0x00,
    bpc = 0x01,
    wl = 0x02,
    kp = 0x03,
    bds = 0x04,
    ka = 0x05,
    bmr = 0x06,
    lg = 0x07,
    ko = 0x08,
    dk = 0x09,
    ys = 0x0A,
    wr = 0x0B,
    blg = 0x0C,
    kk = 0x0D,
    nk = 0x0E,
    ds = 0x0F,
    pc = 0x10,
    ca = 0x11,
    dd = 0x12,
    kt = 0x13,
    jr = 0x14,
    bk = 0x15,
    fk = 0x16,
    rs = 0x17,
    sa_mii_m = 0x18,
    sa_mii_f = 0x19,
    sb_mii_m = 0x1A,
    sb_mii_f = 0x1B,
    sc_mii_m = 0x1C,
    sc_mii_f = 0x1D,
    ma_mii_m = 0x1E,
    ma_mii_f = 0x1F,
    mb_mii_m = 0x20,
    mb_mii_f = 0x21,
    mc_mii_m = 0x22,
    mc_mii_f = 0x23,
    la_mii_m = 0x24,
    la_mii_f = 0x25,
    lb_mii_m = 0x26,
    lb_mii_f = 0x27,
    lc_mii_m = 0x28,
    lc_mii_f = 0x29,
    m_mii = 0x2A,
    s_mii = 0x2B,
    l_mii = 0x2C,
    pc_menu = 0x2D,
    ds_menu = 0x2E,
    rs_menu = 0x2F
};

enum class ControllerType
{
    Wheel = 0x0,
    Nunchuck = 0x1,
    Classic = 0x2,
    GCN = 0x3
};

enum class DriftType
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

public:
    const uint8_t RKGD[4] = {0x52, 0x4B, 0x47, 0x44};

    TimeSignature totalTime;

    TrackID trackId;


    VehicleID vehicleID;
    CharacterID characterID;

    uint8_t year;
    uint8_t month;
    uint8_t day;

    ControllerType controllerId;


    bool isCompressed;

    uint8_t ghostType;
    DriftType driftType;


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

public:
    inline static QString ghostTypeDesc(const uint8_t ghostType)
    {
        if (ghostType == 0x1) return "Players Best Time (PB)";
        if (ghostType == 0x2) return "World Record";
        if (ghostType == 0x3) return "Continental Record";
        if (ghostType == 0x4) return "Rival Ghost";
        if (ghostType == 0x5) return "Special Ghost";
        if (ghostType == 0x6) return "WFC Ghost Race";
        {
            const int subtract = 0x6;
            for (int i = 0x7; i <= 0x24; i++)
            {
                if (ghostType == i) return "Friend Ghost " + QString::number(i - subtract);
            }
        }
        if (ghostType == 0x25) return "Normal Staff Ghost";
        if (ghostType == 0x26) return "Expert Staff Ghost";

        return "";
    }
};
