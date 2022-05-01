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

    // reimplement?
    void adjustInputCenteringMenu(InputFile* inputFile);

    void adjustUiOnFileLoad(InputFile* pInputFile);
    void adjustUiOnFileClose(InputFile* pInputFile);

    void adjustMenuOnClose(InputFile* inputFile);

    void onUndoRedo(InputFile* pInputFile, EOperationType opType);
    void onScroll(InputFile* pInputFile);


    // delete
    void createInputFiles();
    void showError(const QString& errTitle, const QString& errMsg);



    InputFile* playerFile;
    InputFile* ghostFile;

    // done reimplementing ?
    bool userClosedPreviousFile(InputFile* inputFile);

    void openFile(InputFile* inputFile);
    void openFile(InputFile* inputFile, QString filePath);
    void closeFile(InputFile* pInputFile);

    void onReCenter(InputFile* pInputFile);

    // keep ?
    void setTableViewSettings(QTableView* pTable);
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

    void swapModels();


private: // Refactored Functions
    // checkup
    void connectActions();

    void adjustUiOnFileLoad(InputTableView* table, InputFileMenu* menu, const Centering centering);
    void adjustUiOnFileClose(InputTableView* table, InputFileMenu* menu);


    // done
    void setupUi();

    void addMenuItems();
    void addFileMenuItems();

    void setTitles();



    bool userClosedPreviousFile(InputFileHandler** o_fileHandler);

    uint8_t amountLoadedFiles();

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
