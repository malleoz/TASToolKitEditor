#pragma once

#include <Qstack>
#include <QVector>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <QtWidgets/QMainWindow>

typedef QVector<QVector<QString>> TtkFileData;

class InputFile;

class TASToolKitEditor : public QMainWindow
{
    Q_OBJECT

public:
    TASToolKitEditor(QWidget *parent = Q_NULLPTR);

private:
    QAction* actionUndoPlayer;
    QAction* actionRedoPlayer;
    QAction* actionUndoGhost;
    QAction* actionRedoGhost;
    QAction* actionOpenPlayer;
    QAction* actionOpenGhost;
    QAction* actionClosePlayer;
    QAction* actionCloseGhost;
    QAction* action0Centered;
    QAction* action7Centered;
    QAction* actionSwapFiles;
    QAction* actionScrollTogether;
    QWidget* centralWidget;
    QWidget* horizontalLayoutWidget;
    QHBoxLayout* mainHorizLayout;
    QVBoxLayout* playerVLayout;
    QLabel* playerLabel;
    QTableView* playerTableView;
    QVBoxLayout* ghostVLayout;
    QLabel* ghostLabel;
    QTableView* ghostTableView;
    QMenuBar* menuBar;
    QMenu* menuFile;
    QMenu* menuInputCentering;
    QMenu* menuPlayer;
    QMenu* menuGhost;

    InputFile* playerFile;
    InputFile* ghostFile;

    void setupUi();
    void setTitles();
    void setTitleNames();
    void setTitleShortcuts();
    void connectActions();
    void addMenuItems();
    void addFileMenuItems();
    void addPlayerMenuItems();
    void addGhostMenuItems();
    void createInputFileInstances();
    void showError(const QString& errTitle, const QString& errMsg);
    void loadDataToTableView(InputFile* inputFile);

    void onOpenPlayer();
    void onOpenGhost();
    void openFile(InputFile* inputFile);

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

class InputFile
{
public:
    InputFile(QMenu* root, QAction* undo, QAction* redo, QTableView* tableView);

    QString getPath() { return m_filePath; }
    TtkFileData getData() { return m_fileData; }
    QString getCellValue(int rowIdx, int colIdx) { return m_fileData[rowIdx][colIdx]; }
    void setCellValue(int rowIdx, int colIdx, QString value) { m_fileData[rowIdx][colIdx] = value; }
    FileStatus loadFile(QString path);
    void closeFile();
    Centering getCentering() { return m_fileCentering; }
    void setTableView(QTableView* tableView) { pTableView = tableView; }
    QTableView* getTableView() { return pTableView; }

private:

    QString m_filePath;
    TtkFileData m_fileData;
    Centering m_fileCentering;
    bool m_tableViewLoaded;
    QStack<CellEditAction> m_undoStack;
    QStack<CellEditAction> m_redoStack;
    QTableView* pTableView;
    QMenu* pRootMenu;
    QAction* pUndoMenu;
    QAction* pRedoMenu;
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

class InputFileModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    InputFileModel(InputFile* pFile, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
    InputFile* m_pFile;
};