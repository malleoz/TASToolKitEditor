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

    int m_filesLoaded;

    void setupUi();
    void finishSettingUpTableViews();
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
    bool userClosedPreviousFile(InputFile* inputFile);
    void adjustInputCenteringMenu(InputFile* inputFile);
    void setTableViewSettings(QTableView* pTable);
    void adjustUiOnFileLoad(InputFile* pInputFile);

    void onOpenPlayer();
    void onOpenGhost();
    void openFile(InputFile* inputFile);
    void onClosePlayer();
    void onCloseGhost();
    void closeFile(InputFile* pInputFile);
};