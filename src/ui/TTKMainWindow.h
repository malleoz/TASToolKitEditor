#pragma once

#include <QStack>
#include <QVector>

#include <QtWidgets/QAction>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

#include <QtWidgets/QMainWindow>

#include "include/Definitions.h"
#include "data/PlayerTypeInstance.h"

class InputFileMenu;
class InputTableView;
class InputFileHandler;


class TTKMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    TTKMainWindow(QWidget *parent = Q_NULLPTR);


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

    void onToggleScrollTogether(const bool bTogether);

    void swapModels();

    void adjustUiOnClose();


private: // Refactored Functions
    // checkup
    void connectActions();

    // done
    void setupUi();

    void addMenuItems();
    void addFileMenuItems();

    void setTitles();

    bool bothFilesLoaded();

private: // variables
    PlayerTypeInstance m_player;
    PlayerTypeInstance m_ghost;

};
