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

    InputFile* playerFile;
    InputFile* ghostFile;


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
    void onReCenter(InputFile* pInputFile);
    void scrollToFirstTable(QTableView* dst, QTableView* src);

private: // Qt UI Elements
    QWidget* centralWidget;
    QWidget* horizontalLayoutWidget;

    QMenuBar* menuBar;
    QMenu* menuFile;

    QAction* actionOpenPlayer;
    QAction* actionOpenGhost;
    QAction* actionSwapFiles;
    QAction* actionScrollTogether;

    QHBoxLayout* mainHorizLayout;

    QVBoxLayout* playerVLayout;
    QLabel* playerLabel;
    QVBoxLayout* ghostVLayout;
    QLabel* ghostLabel;


private: // connect
    void openFile(InputFileHandler** o_fileHandler, InputTableView* table, InputFileMenu* menu);
    void closeFile(InputFileHandler** o_fileHandler, InputTableView* table, InputFileMenu* menu);

    void toggleCentering(InputTableView* table);

    void onScroll();
    void onToggleScrollTogether(const bool bTogether);

    void onUndoRedo(InputTableView* table, const EOperationType opType);


private: // Refactored Functions
    void setupUi();

    void addMenuItems();
    void addFileMenuItems();

    void setTitles();
    void connectActions();

    bool userClosedPreviousFile(InputFileHandler** o_fileHandler);

    void adjustUiOnFileLoad(InputTableView* table, InputFileMenu* menu);
    void adjustUiOnFileClose(InputTableView* table, InputFileMenu* menu);

private: // variables
    InputTableView* m_pPlayerTableView;
    InputTableView* m_pGhostTableView;

    InputFileHandler* m_pPlayerFileHandler;
    InputFileHandler* m_pGhostFileHandler;

    InputFileMenu* m_pPlayerMenu;
    InputFileMenu* m_pGhostMenu;

    bool m_bScrollTogether;
    int m_filesLoaded;
};
