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
    static constexpr TrackID TRACK_ID_ORDERED_LIST[32] =
    {
        TrackID::LC, TrackID::MMM, TrackID::MG, TrackID::TF,
        TrackID::MC, TrackID::CM, TrackID::DKS, TrackID::WGM,
        TrackID::DC, TrackID::KC, TrackID::MT, TrackID::GV,
        TrackID::DDR, TrackID::MH, TrackID::BC, TrackID::RR,

        TrackID::rPB, TrackID::rYF, TrackID::rGV2, TrackID::rMR,
        TrackID::rSL, TrackID::rSGB, TrackID::rDS, TrackID::rWS,
        TrackID::rDH, TrackID::rBC3, TrackID::rDKJP, TrackID::rMC,
        TrackID::rMC3, TrackID::rPG, TrackID::rDKM, TrackID::rBC
    };

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

    inline static QString trackDesc(const TrackID trackID)
    {
        switch (trackID)
        {
            case TrackID::LC: return "Luigi Circuit";
            case TrackID::MMM: return "Moo Moo Meadows";
            case TrackID::MG: return "Mushroom Gorge";
            case TrackID::TF: return "Toads Factory";

            case TrackID::MC: return "Mario Circuit";
            case TrackID::CM: return "Coconut Mall";
            case TrackID::DKS: return "DK Summit";
            case TrackID::WGM: return "Warios Gold Mine";

            case TrackID::DC: return "Daisy Circuit";
            case TrackID::KC: return "Koopa Cape";
            case TrackID::MT: return "Maple Treeway";
            case TrackID::GV: return "Grumble Volcano";

            case TrackID::DDR: return "Dry Dry Ruins";
            case TrackID::MH: return "Moonview Highway";
            case TrackID::BC: return "Bowser Castle";
            case TrackID::RR: return "Rainbow Road";

            case TrackID::rPB: return "GCN Peach Beach";
            case TrackID::rYF: return "DS Yoshi Falls";
            case TrackID::rGV2: return "SNES Ghost Valley 2";
            case TrackID::rMR: return "N64 Mario Raceway";

            case TrackID::rSL: return "N64 Sherbet Land";
            case TrackID::rSGB: return "GBA Shy Guy Beach";
            case TrackID::rDS: return "DS Delfino Square";
            case TrackID::rWS: return "GCN Waluigi Stadium";

            case TrackID::rDH: return "DS Desert Hills";
            case TrackID::rBC3: return "GBA Bowsers Castle 3";
            case TrackID::rDKJP: return "N64 Jungle Parkway";
            case TrackID::rMC: return "GCN Mario Circuit";

            case TrackID::rMC3: return "SNES Mario Circuit 3";
            case TrackID::rPG: return "DS Peach Gardens";
            case TrackID::rDKM: return "GCN DK Mountain";
            case TrackID::rBC: return "N64 Bowsers Castle";
        }
    }

    inline static QString vehicleDesc(const VehicleID vehicleID)
    {
        switch (vehicleID)
        {
            case VehicleID::sdf_kart: return "Standard Kart S";
            case VehicleID::mdf_kart: return "Standard Kart M";
            case VehicleID::ldf_kart: return "Standard Kart L";
            case VehicleID::sa_kart: return "Booster Seat";
            case VehicleID::ma_kart: return "Classic Dragster";
            case VehicleID::la_kart: return "Offroader";
            case VehicleID::sb_kart: return "Mini Beast";
            case VehicleID::mb_kart: return "Wild Wing";
            case VehicleID::lb_kart: return "Flame Fyler";
            case VehicleID::sc_kart: return "Cheep Charger";
            case VehicleID::mc_kart: return "Super Blooper";
            case VehicleID::lc_kart: return "Piranha Prowler";
            case VehicleID::sd_kart: return "Tiny Titan";
            case VehicleID::md_kart: return "Daytripper";
            case VehicleID::ld_kart: return "Jetsetter";
            case VehicleID::se_kart: return "Blue Falcon";
            case VehicleID::me_kart: return "Sprinter";
            case VehicleID::le_kart: return "Honeycoupe";
            case VehicleID::sdf_bike: return "Standard Bike S";
            case VehicleID::mdf_bike: return "Standard Bike M";
            case VehicleID::ldf_bike: return "Standard Bike L";
            case VehicleID::sa_bike: return "Bullet Bike";
            case VehicleID::ma_bike: return "Mach Bike";
            case VehicleID::la_bike: return "Flame Runner";
            case VehicleID::sb_bike: return "Bit Bike";
            case VehicleID::mb_bike: return "Sugarscoot";
            case VehicleID::lb_bike: return "Wario Bike";
            case VehicleID::sc_bike: return "Quacker";
            case VehicleID::mc_bike: return "Zip Zip";
            case VehicleID::lc_bike: return "Shooting Star";
            case VehicleID::sd_bike: return "Magikruiser";
            case VehicleID::md_bike: return "Sneakster";
            case VehicleID::ld_bike: return "Spear";
            case VehicleID::se_bike: return "Jet Bubble";
            case VehicleID::me_bike: return "Dolphin Dasher";
            case VehicleID::le_bike: return "Phantom";
        }
    }

    inline static QString characterDesc(const CharacterID characterID)
    {
        switch (characterID)
        {
            case CharacterID::mr: return "Mario";
            case CharacterID::bpc: return "Baby Peach";
            case CharacterID::wl: return "Waluigi";
            case CharacterID::kp: return "Bowser";
            case CharacterID::bds: return "Baby Daisy";
            case CharacterID::ka: return "Dry Bones";
            case CharacterID::bmr: return "Baby Mario";
            case CharacterID::lg: return "Luigi";
            case CharacterID::ko: return "Toad";
            case CharacterID::dk: return "Donkey Kong";
            case CharacterID::ys: return "Yoshi";
            case CharacterID::wr: return "Wario";
            case CharacterID::blg: return "Baby Luigi";
            case CharacterID::kk: return "Toadette";
            case CharacterID::nk: return "Koopa Troopa";
            case CharacterID::ds: return "Daisy";
            case CharacterID::pc: return "Peach";
            case CharacterID::ca: return "Birdo";
            case CharacterID::dd: return "Diddy Kong";
            case CharacterID::kt: return "King Boo";
            case CharacterID::jr: return "Bowser Jr.";
            case CharacterID::bk: return "Dry Bowser";
            case CharacterID::fk: return "Funky Kong";
            case CharacterID::rs: return "Rosalina";
            case CharacterID::sa_mii_m: return "Small Mii Outfit A (Male)";
            case CharacterID::sa_mii_f: return "Small Mii Outfit A (Female)";
            case CharacterID::sb_mii_m: return "Small Mii Outfit B (Male)";
            case CharacterID::sb_mii_f: return "Small Mii Outfit B (Female)";
            case CharacterID::sc_mii_m: return "Small Mii Outfit C (Male)";
            case CharacterID::sc_mii_f: return "Small Mii Outfit C (Female)";
            case CharacterID::ma_mii_m: return "Medium Mii Outfit A (Male)";
            case CharacterID::ma_mii_f: return "Medium Mii Outfit A (Female)";
            case CharacterID::mb_mii_m: return "Medium Mii Outfit B (Male)";
            case CharacterID::mb_mii_f: return "Medium Mii Outfit B (Female)";
            case CharacterID::mc_mii_m: return "Medium Mii Outfit C (Male)";
            case CharacterID::mc_mii_f: return "Medium Mii Outfit C (Female)";
            case CharacterID::la_mii_m: return "Large Mii Outfit A (Male)";
            case CharacterID::la_mii_f: return "Large Mii Outfit A (Female)";
            case CharacterID::lb_mii_m: return "Large Mii Outfit B (Male)";
            case CharacterID::lb_mii_f: return "Large Mii Outfit B (Female)";
            case CharacterID::lc_mii_m: return "Large Mii Outfit C (Male)";
            case CharacterID::lc_mii_f: return "Large Mii Outfit C (Female)";
            case CharacterID::m_mii: return "Medium Mii";
            case CharacterID::s_mii: return "Small Mii";
            case CharacterID::l_mii: return "Large Mii";
            case CharacterID::pc_menu: return "Peach Biker Outfit";
            case CharacterID::ds_menu: return "Daisy Biker Outfit";
            case CharacterID::rs_menu: return "Rosalina Biker Outfit";
        }
    }

    inline static QString controllerDesc(const ControllerType controllerID)
    {
        switch (controllerID)
        {
            case ControllerType::Wheel: return "Wii Wheel";
            case ControllerType::Nunchuck: return "Wii Remote and Nunchuck";
            case ControllerType::Classic: return "Classic Controller";
            case ControllerType::GCN: return "Gamecube Controller";
        }
    }

    inline static QString driftDesc(const DriftType driftID)
    {
        switch (driftID)
        {
            case DriftType::Manual: return "Manual";
            case DriftType::Automatic: return "Automatic";
        }
    }

};
