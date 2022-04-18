#pragma once

#include <QStack>

#define NUM_INPUT_COLUMNS 6
#define FRAMECOUNT_COLUMN 1

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

class CellEditAction
{
public:
    CellEditAction();

    bool operator==(const CellEditAction& rhs);

private:
    int m_rowIdx;
    int m_colIdx;
    int m_prev;
    int m_cur;

    void flipValues();
};

typedef QVector<QVector<QString>> TtkFileData;

class QAction;
class QMenu;
class QTableView;

struct InputFileMenus
{
    InputFileMenus(QMenu* root, QAction* undo, QAction* redo, QAction* close)
        : root(root)
        , undo(undo)
        , redo(redo)
        , close(close)
    {
    }

    QMenu* root;
    QAction* undo;
    QAction* redo;
    QAction* close;
};


class InputFile
{
public:
    InputFile(const InputFileMenus& menus, QTableView* tableView);

    QString getPath() { return m_filePath; }
    TtkFileData getData() { return m_fileData; }
    QString getCellValue(int rowIdx, int colIdx) { return m_fileData[rowIdx][colIdx]; }
    void setCellValue(int rowIdx, int colIdx, QString value) { m_fileData[rowIdx][colIdx] = value; }
    FileStatus loadFile(QString path);
    void closeFile();
    Centering getCentering() { return m_fileCentering; }
    void setTableView(QTableView* tableView) { pTableView = tableView; }
    QTableView* getTableView() { return pTableView; }
    const InputFileMenus& getMenus() { return m_menus; }

private:

    QString m_filePath;
    TtkFileData m_fileData;
    Centering m_fileCentering;
    bool m_tableViewLoaded;
    QStack<CellEditAction> m_undoStack;
    QStack<CellEditAction> m_redoStack;
    QTableView* pTableView;
    InputFileMenus m_menus;
    int m_frameParseError;

    bool valuesFormattedProperly(const QStringList& data);
    bool valueRestrictionsAreMet(const QStringList& data);
    int getSmallestAcceptedValue(int index, int value);
    int getLargestAcceptedValue(int index, int value);
    bool ableToDiscernCentering(int value);
    void clearData();

    const QVector<int> BUTTON_COL_IDXS{ 0, 1, 2 };
    const int DPAD_COL_IDX = 5;
};

