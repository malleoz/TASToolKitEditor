#pragma once

#include <Qstack>
#include <QStyledItemDelegate>
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

class InputFile;
enum class EOperationType;
enum class Centering;

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
    QAction* action0CenteredPlayer;
    QAction* action0CenteredGhost;
    QAction* action7CenteredPlayer;
    QAction* action7CenteredGhost;
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
    QMenu* menuCenterPlayer;
    QMenu* menuCenterGhost;
    QMenu* menuPlayer;
    QMenu* menuGhost;

    InputFile* playerFile;
    InputFile* ghostFile;

    int m_filesLoaded;
    bool m_bScrollTogether;

    void setupUi();
    void setTitles();
    void setTitleNames();
    void setTitleShortcuts();
    void connectActions();
    void addMenuItems();
    void addFileMenuItems();
    void addPlayerMenuItems();
    void addGhostMenuItems();
    void createInputFiles();
    void showError(const QString& errTitle, const QString& errMsg);
    bool userClosedPreviousFile(InputFile* inputFile);
    void adjustInputCenteringMenu(InputFile* inputFile);
    void setTableViewSettings(QTableView* pTable);
    void adjustUiOnFileLoad(InputFile* pInputFile);
    void adjustUiOnFileClose(InputFile* pInputFile);
    void adjustMenuOnClose(InputFile* inputFile);

    void openFile(InputFile* inputFile);
    void openFile(InputFile* inputFile, QString filePath);
    void closeFile(InputFile* pInputFile);
    void onUndoRedo(InputFile* pInputFile, EOperationType opType);
    void onScroll(InputFile* pInputFile);
    void onToggleScrollTogether(bool bTogether);
    void onReCenter(InputFile* pInputFile, Centering centering);
    void scrollToFirstTable(QTableView* dst, QTableView* src);
};