#pragma once

#include <QStack>
#include <QObject>

#include "include/Definitions.h"


class QFileSystemWatcher;


class InputFileHandler : public QObject
{
public:
    InputFileHandler(QString path, QObject* parent = nullptr);
    virtual ~InputFileHandler() override;

    FileStatus loadFile(TTKFileData& o_emptyTTK, Centering& o_centering);
    void saveFile(const TTKFileData& fileData);


    inline QString getPath() const { return m_filePath; }
    inline QFileSystemWatcher* getFsWatcher() const { return m_pFsWatcher; }

private:
    static ParseStatus checkFormatting(const QStringList& data, const Centering centering);
    Centering getCentering(const QStringList& data) const;

private:
    QString m_filePath;
    QFileSystemWatcher* m_pFsWatcher;
};

