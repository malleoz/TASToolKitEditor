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

#include "Definitions.h"

class InputFile;
class InputFileMenu;
class InputTableView;
class InputFileHandler;

enum class EOperationType;
enum class Centering;

class TTKMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TTKMainWindow(QWidget *parent = Q_NULLPTR);

private:
    InputFileMenu* m_pPlayerMenu;
    InputFileMenu* m_pGhostMenu;

    QAction* actionOpenPlayer;
    QAction* actionOpenGhost;
    QAction* actionSwapFiles;
    QAction* actionScrollTogether;
    QWidget* centralWidget;
    QWidget* horizontalLayoutWidget;
    QHBoxLayout* mainHorizLayout;
    QVBoxLayout* playerVLayout;
    QLabel* playerLabel;
    QVBoxLayout* ghostVLayout;
    QLabel* ghostLabel;
    QMenuBar* menuBar;
    QMenu* menuFile;

    InputFile* playerFile;
    InputFile* ghostFile;


    void setupUi();
    void setTitles();
    void connectActions();
    void addMenuItems();
    void addFileMenuItems();
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
    void onReCenter(InputFile* pInputFile);
    void scrollToFirstTable(QTableView* dst, QTableView* src);

private:
    void openFile(FileType type);
    void closeFile(FileType type);

    void toggleCentering();

    void onScroll();



private:
    InputTableView* playerTableView;
    InputTableView* ghostTableView;

    InputFileHandler* playerFileHandler;
    InputFileHandler* ghostFileHandler;

    bool m_bScrollTogether;
    int m_filesLoaded;
};
