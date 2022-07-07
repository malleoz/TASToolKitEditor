#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QStringList>
#include <QVariant>
#include <QVector>

#define NUM_INPUT_COLUMNS 6
#define FRAMECOUNT_COLUMN 1
#define DPAD_COL_IDX 5

#define RESET_MACRO 0x514D

enum class FileStatus
{
    Success = 0,
    Canceled,
    InsufficientWritePermission,
    Parse,
    FileSizeLimit
};

enum class ParseStatus
{
    Success = 0,
    NumColumnsError,
    NegativeZeroError,
    ConvertError,
    CenteringError,
    ButtonValueError,
    DPadValueError,
};

enum class Centering
{
    Unknown = 0,
    Seven,
    Zero,
};

enum class EOperationType
{
    Normal = 0,
    Undo,
    Redo,
};

// ToDo rename/replace
// "FileTypes" would be *.csv *.rkg etc
enum class FileType
{
    Player = 0,
    Ghost,
};

typedef QVector<QString> FrameData;
typedef QVector<FrameData> TTKFileData;

// Clarification typedef
typedef TTKFileData CSVData;


class DefinitionUtils
{
private:
    inline DefinitionUtils() {}

public:
    inline static bool CheckButtons(const QStringList& data)
    {
        for(QString value : data)
        {
            if (value != "0" && value != "1")
                return false;
        }

        return true;
    }
    
    inline static bool CheckCentering(const Centering centering, const int32_t value)
    {
        switch(centering)
        {
            case Centering::Seven: return value > 14 ? false : (value >= 0);
            case Centering::Zero: return value > 7 ? false : (value >= -7);
            case Centering::Unknown: return value > 14 ? false : (value >= -7);
        }

        return false;
    }

    inline static bool CheckDPad(QString pad)
    {
        int iPad = pad.toInt();

        return iPad >= 0 && iPad <= 4;
    }

    inline static QString TranslateParseStatusToErrorString(ParseStatus status, uint32_t frame)
    {
        switch (status)
        {
        case ParseStatus::ConvertError:
            return QString("Could not convert value on frame %1 to an integer.").arg(frame);
        case ParseStatus::NegativeZeroError:
            return QString("Invalid value '-0' on frame %1.").arg(frame);
        case ParseStatus::NumColumnsError:
            return QString("Expected 6 comma-separated values on frame %1.").arg(frame);
        case ParseStatus::ButtonValueError:
            return QString("Invalid value for A, B, or L button on frame %1.").arg(frame);
        case ParseStatus::CenteringError:
            return QString("Value is not within the centering range on frame %1.").arg(frame);
        case ParseStatus::DPadValueError:
            return QString("Invalid value for DPad on frame %1.").arg(frame);
        default:
            return QString("Unhandled error on frame %1.").arg(frame);
        }
    }

    inline static FrameData GetDefaultFrameData(const Centering centering)
    {
        QString c = centering == Centering::Seven ? "7" : "0";

        FrameData frameData =
        {
            "0",
            "0",
            "0",
            c,
            c,
            "0"
        };

        return frameData;
    }
};

#endif // DEFINITIONS_H
