#pragma once

#include <QStack>

#define NUM_INPUT_COLUMNS 6
#define FRAMECOUNT_COLUMN 1
#define DPAD_COL_IDX 5

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
    CellEditAction(int row = 0, int col = 0, QString prev = "", QString cur = "");

    bool operator==(const CellEditAction& rhs);
    inline void flipValues()
    {
        QString temp = m_cur;
        m_cur = m_prev;
        m_prev = temp;
    }
    inline int row() { return m_rowIdx; }
    inline int col() { return m_colIdx; }
    inline QString curVal() { return m_cur; }

private:
    int m_rowIdx;
    int m_colIdx;
    QString m_prev;
    QString m_cur;
};

enum class EOperationType
{
    Normal = 0,
    Undo,
    Redo,
};

typedef QVector<QVector<QString>> TtkFileData;
typedef QStack<CellEditAction> TtkStack;

class QAction;
class QFileSystemWatcher;
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
    void setCentering(Centering center);
    inline void setTableView(QTableView* tableView) { pTableView = tableView; }
    inline QTableView* getTableView() { return pTableView; }
    const inline InputFileMenus& getMenus() { return m_menus; }
    inline QLabel* getLabel() { return pLabel; }
    bool inputValid(const QModelIndex& index, const QVariant& value);
    inline int getParseError() { return m_frameParseError; }
    inline QStack<CellEditAction>* getUndoStack() { return &m_undoStack; }
    inline QStack<CellEditAction>* getRedoStack() { return &m_redoStack; }
    inline QFileSystemWatcher* getFsWatcher() { return m_pFsWatcher; }
    void fileChanged();
    void onCellClicked(const QModelIndex& index);
    void swap(InputFile* rhs);
    inline void setModified(bool bSet) { m_bModified = bSet; }
    void applyStickOffset(int offset);

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
    QFileSystemWatcher* m_pFsWatcher;
    bool m_bModified;
    int m_iModifiedCheck;

    bool valuesFormattedProperly(const QStringList& data);
    bool valueRestrictionsAreMet(const QStringList& data);
    int getSmallestAcceptedValue(int index, int value);
    int getLargestAcceptedValue(int index, int value);
    bool ableToDiscernCentering(int value);
    void clearData();
    void onClickFramecountColumn(const QModelIndex& index);
    void onClickButtonColumn(const QModelIndex& index);

    const QVector<int> BUTTON_COL_IDXS{ 0, 1, 2 };
};

