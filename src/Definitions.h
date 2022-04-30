#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QVector>

enum class FileStatus
{
    Success = 0,
    WritePermission,
    Parse,
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
};

#endif // DEFINITIONS_H
