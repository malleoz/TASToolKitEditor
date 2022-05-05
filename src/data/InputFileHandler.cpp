#include "InputFileHandler.h"

#include <QFile>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QTextStream>


InputFileHandler::InputFileHandler(QString path, QObject* parent)
    : QObject(parent)
    , m_filePath(path)
{
    m_pFsWatcher = new QFileSystemWatcher(QStringList(m_filePath));
}

InputFileHandler::~InputFileHandler()
{
    delete m_pFsWatcher;
    m_pFsWatcher = nullptr;
}

FileStatus InputFileHandler::loadFile(TTKFileData& o_emptyTTK, Centering& o_centering)
{
    QFile fp(m_filePath);

    if (!fp.open(QIODevice::ReadWrite))
    {
        const QString errorTitle = "Error Opening File";
        const QString errorMsg = "This program does not have sufficient permissions to modify the file.\n\n" \
            "Try running this program in administrator mode and make sure the file is not open in another program.";
        QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg, QMessageBox::StandardButton::Ok);

        fp.close();
        return FileStatus::InsufficientWritePermission;
    }

    QTextStream ts(&fp);

    o_centering = Centering::Unknown;

    while (!ts.atEnd())
    {
        const QString line = ts.readLine();
        const QStringList frameData = line.split(',');

        const ParseStatus formatStatus = checkFormatting(frameData, o_centering);


        if (formatStatus != ParseStatus::Success)
        {
            const uint32_t errorFrame = static_cast<uint32_t>(o_emptyTTK.count());
            const QString errorTitle = "Error Parsing File";
            const QString errorMsg = DefinitionUtils::TranslateParseStatusToErrorString(formatStatus, errorFrame);
            QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg, QMessageBox::StandardButton::Ok);

            m_filePath = "";
            o_emptyTTK.clear();

            fp.close();
            return FileStatus::Parse;
        }

        if (o_centering == Centering::Unknown)
        {
            o_centering = getCentering(frameData);
        }
        o_emptyTTK.append(frameData.toVector());
    }

    fp.close();

    return FileStatus::Success;
}

void InputFileHandler::saveFile(const TTKFileData& fileData)
{
    // Disconnect FileSystemWatcher so it doesn't detect saved changes
    m_pFsWatcher->removePath(m_filePath);

    QFile fp(m_filePath);

    if (!fp.open(QFile::WriteOnly))
    {
        const QString errorTitle = "Error Saving File";
        const QString errorMsg = "This program does not have sufficient permissions to modify the file.\n\n" \
            "Your change has not been saved to the file.";
        QMessageBox::warning(Q_NULLPTR, errorTitle, errorMsg, QMessageBox::StandardButton::Ok);
        return;
    }

    QTextStream os(&fp);
    for (int i = 0; i < fileData.count(); i++)
    {
        for (int j = 0; j < NUM_INPUT_COLUMNS; j++)
        {
            os << fileData[i][j];

            if (j == NUM_INPUT_COLUMNS - 1)
                os << '\n';
            else
                os << ',';
        }
    }

    os.flush();

    fp.close();

    // Reconnect the FileSystemWatcher after saving and closing
    m_pFsWatcher->addPath(m_filePath);
}


/*static*/ ParseStatus InputFileHandler::checkFormatting(const QStringList& data, const Centering centering)
{
    // There should be 6 comma-separated values per line
    if (data.count() != NUM_INPUT_COLUMNS)
        return ParseStatus::NumColumnsError;

    for (int i = 0; i < data.count(); i++)
    {
        // -0 actually succeeds in the toInt() conversion
        // so we need to manually catch it, since this value
        // won't work when used in Dolphin
        if (data[i] == "-0")
            return ParseStatus::NegativeZeroError;

        bool ret;
        data[i].toInt(&ret);

        if (!ret)
            return ParseStatus::ConvertError;
    }

    if (!DefinitionUtils::CheckButtons(data.mid(0, 3)))
        return ParseStatus::ButtonValueError;

    if (!DefinitionUtils::CheckCentering(centering, data[3].toInt()))
        return ParseStatus::CenteringError;

    if (!DefinitionUtils::CheckCentering(centering, data[4].toInt()))
        return ParseStatus::CenteringError;

    if (!DefinitionUtils::CheckDPad(data[5]))
        return ParseStatus::DPadValueError;


    // Place other error checks here

    return ParseStatus::Success;
}


Centering InputFileHandler::getCentering(const QStringList& data) const
{
    for (int i = 3; i <= 4; i++)
    {
        int value = data[i].toInt();

        if (value < 0) return Centering::Zero;
        if (value > 7) return Centering::Seven;
    }

    return Centering::Unknown;
}
