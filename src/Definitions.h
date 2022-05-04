#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QVector>

enum class FileStatus
{
    Success = 0,
    InsufficientWritePermission,
    Parse,
};

enum class ParseStatus
{
    Success = 0,
    NumColumnsError,
    NegativeZeroError,
    ConvertError,
    CenteringError,
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


class DefinitionUtils
{
private:
    inline DefinitionUtils() {}

public:
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

    inline static QString TranslateParseStatusToErrorString(ParseStatus status, uint32_t frame)
    {
        switch (status)
        {
        case ParseStatus::CenteringError:
            return QString("Value is not within the centering range on frame %1.").arg(frame);
        case ParseStatus::ConvertError:
            return QString("Could not convert value on frame %1 to an integer.").arg(frame);
        case ParseStatus::NegativeZeroError:
            return QString("Invalid value '-0' on frame %1.").arg(frame);
        case ParseStatus::NumColumnsError:
            return QString("Expected 6 comma-separated values on frame %1.").arg(frame);
        default:
            return QString("Unhandled error on frame %1.").arg(frame);
        }
    }
};

#endif // DEFINITIONS_H
