#pragma once

#include <QStack>
#include <QTableView>

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

enum class EOperationType
{
    Normal = 0,
    Undo,
    Redo,
};

typedef QVector<QString> FrameData;
typedef QVector<QVector<QString>> TtkFileData;

class QAction;
class QFileSystemWatcher;
class QLabel;
class QMenu;
class QModelIndex;
class QVariant;
class QWidget;

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

/// Custom QTableView to implement QoL features
class InputTableView : public QTableView
{
public:
    InputTableView(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
};


/// Data manager and controller for the \ref<InputTableView>
class InputFile
{
public:
    /// Class for storing latest table edits
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

    typedef QStack<CellEditAction> TtkStack;

public:
    InputFile(const InputFileMenus& menus, QLabel* label, InputTableView* tableView);

    inline QString getPath() { return m_filePath; }
    const inline TtkFileData& getData() { return m_fileData; }
    inline QString getCellValue(int rowIdx, int colIdx) { return m_fileData[rowIdx][colIdx]; }
    inline void setCellValue(int rowIdx, int colIdx, QString value) { m_fileData[rowIdx][colIdx] = value; }

    FileStatus loadFile(QString path);
    void closeFile();
    void fileChanged();

    inline Centering getCentering() { return m_fileCentering; }
    void setCentering(Centering center);
    inline void setTableView(InputTableView* tableView) { pTableView = tableView; }
    inline InputTableView* getTableView() { return pTableView; }
    const inline InputFileMenus& getMenus() { return m_menus; }
    inline QLabel* getLabel() { return pLabel; }

    bool inputValid(const QModelIndex& index, const QVariant& value);

    inline int getParseError() { return m_frameParseError; }
    inline QStack<CellEditAction>* getUndoStack() { return &m_undoStack; }
    inline QStack<CellEditAction>* getRedoStack() { return &m_redoStack; }
    inline QFileSystemWatcher* getFsWatcher() { return m_pFsWatcher; }

    inline void setModified(bool bSet) { m_bModified = bSet; }
    inline QString getParseMsg() { return m_sParseErrorVal; }

    void onCellClicked(const QModelIndex& index);
    void swap(InputFile* rhs);

    void applyStickOffset(int offset);
    void addData(int rowTemplate, int atRow, int count);

private:

    QString m_filePath;
    TtkFileData m_fileData;

    Centering m_fileCentering;
    bool m_tableViewLoaded;

    QStack<CellEditAction> m_undoStack;
    QStack<CellEditAction> m_redoStack;

    InputTableView* pTableView;
    InputFileMenus m_menus;
    QLabel* pLabel;

    int m_frameParseError;
    QString m_sParseErrorVal;
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

