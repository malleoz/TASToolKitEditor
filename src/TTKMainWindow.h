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
#include "PlayerTypeInstance.h"

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

    void onScroll(InputFile* pInputFile);

    InputFile* playerFile;
    InputFile* ghostFile;

    // keep ?
    void scrollToFirstTable(InputTableView* dst, InputTableView* src);


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


private: // connect
    void openFile(PlayerTypeInstance& typeInstance);
    void closeFile(PlayerTypeInstance& typeInstance);

    void onScroll();
    void onToggleScrollTogether(const bool bTogether);


    void swapModels();


private: // Refactored Functions
    // checkup
    void connectActions();

    // done
    void setupUi();

    void addMenuItems();
    void addFileMenuItems();

    void setTitles();

    uint8_t amountLoadedFiles();

private: // variables
    PlayerTypeInstance m_player;
    PlayerTypeInstance m_ghost;

    bool m_bScrollTogether;

};
