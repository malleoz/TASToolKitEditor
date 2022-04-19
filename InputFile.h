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
class QLabel;
class QMenu;
class QModelIndex;
class QTableView;
class QVariant;

struct InputFileMenus
{
    InputFileMenus(QMenu* root, QAction* undo, QAction* redo, QAction* close, QAction* center0, QAction* center7)
        : root(root)
        , undo(undo)
        , redo(redo)
        , close(close)
        , center0(center0)
        , center7(center7)
    {
    }

    QMenu* root;
    QAction* undo;
    QAction* redo;
    QAction* close;
    QAction* center0;
    QAction* center7;
};


class InputFile
{
public:
    InputFile(const InputFileMenus& menus, QLabel* label, QTableView* tableView);

    inline QString getPath() { return m_filePath; }
    const inline TtkFileData& getData() { return m_fileData; }
    inline QString getCellValue(int rowIdx, int colIdx) { return m_fileData[rowIdx][colIdx]; }
    inline void setCellValue(int rowIdx, int colIdx, QString value) { m_fileData[rowIdx][colIdx] = value; }
    FileStatus loadFile(QString path);
    void closeFile();
    inline Centering getCentering() { return m_fileCentering; }
    inline void setTableView(QTableView* tableView) { pTableView = tableView; }
    inline QTableView* getTableView() { return pTableView; }
    const inline InputFileMenus& getMenus() { return m_menus; }
    inline QLabel* getLabel() { return pLabel; }
    bool inputValid(const QModelIndex& index, int value);

private:

    QString m_filePath;
    TtkFileData m_fileData;
    Centering m_fileCentering;
    bool m_tableViewLoaded;
    QStack<CellEditAction> m_undoStack;
    QStack<CellEditAction> m_redoStack;
    QTableView* pTableView;
    QLabel* pLabel;
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

