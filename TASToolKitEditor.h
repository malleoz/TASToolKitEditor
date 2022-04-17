#pragma once

#include <stack>

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

class InputFile
{
public:
    InputFile(QMenu* root, QAction* undo, QAction* redo);

private:

    std::string m_filePath;
    std::vector<std::vector<int>> m_fileData;
    bool m_tableViewLoaded;
    std::stack<CellEditAction*> m_undoStack;
    std::stack<CellEditAction*> m_redoStack;
    QTableView* pTableView;
    QMenu* pRootMenu;
    QAction* pUndoMenu;
    QAction* pRedoMenu;
};