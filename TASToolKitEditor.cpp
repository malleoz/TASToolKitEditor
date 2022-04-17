#include "TASToolKitEditor.h"

TASToolKitEditor::TASToolKitEditor(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    connectActions();
}

void TASToolKitEditor::connectActions()
{

}

void TASToolKitEditor::setupUi()
{
    resize(452, 405);
    actionUndoPlayer = new QAction(this);
    actionRedoPlayer = new QAction(this);
    actionUndoGhost = new QAction(this);
    actionRedoGhost = new QAction(this);
    actionOpenPlayer = new QAction(this);
    actionOpenGhost = new QAction(this);
    actionClosePlayer = new QAction(this);
    actionCloseGhost = new QAction(this);
    action0Centered = new QAction(this);
    action7Centered = new QAction(this);
    actionSwapFiles = new QAction(this);
    actionScrollTogether = new QAction(this);
    centralWidget = new QWidget(this);
    horizontalLayoutWidget = new QWidget(centralWidget);
    horizontalLayoutWidget->setGeometry(QRect(9, 0, 351, 341));
    mainHorizLayout = new QHBoxLayout(horizontalLayoutWidget);
    mainHorizLayout->setSpacing(6);
    mainHorizLayout->setContentsMargins(11, 11, 11, 11);
    mainHorizLayout->setContentsMargins(0, 0, 0, 0);
    playerVLayout = new QVBoxLayout();
    playerVLayout->setSpacing(6);
    playerLabel = new QLabel(horizontalLayoutWidget);

    playerVLayout->addWidget(playerLabel);

    playerTableView = new QTableView(horizontalLayoutWidget);

    playerVLayout->addWidget(playerTableView);


    mainHorizLayout->addLayout(playerVLayout);

    ghostVLayout = new QVBoxLayout();
    ghostVLayout->setSpacing(6);
    ghostLabel = new QLabel(horizontalLayoutWidget);

    ghostVLayout->addWidget(ghostLabel);

    ghostTableView = new QTableView(horizontalLayoutWidget);

    ghostVLayout->addWidget(ghostTableView);


    mainHorizLayout->addLayout(ghostVLayout);

    setCentralWidget(centralWidget);
    menuBar = new QMenuBar(this);
    menuBar->setGeometry(QRect(0, 0, 452, 21));
    menuFile = new QMenu(menuBar);
    menuInputCentering = new QMenu(menuFile);
    menuPlayer = new QMenu(menuBar);
    menuGhost = new QMenu(menuBar);
    setMenuBar(menuBar);
    mainToolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, mainToolBar);
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    menuBar->addAction(menuFile->menuAction());
    menuBar->addAction(menuPlayer->menuAction());
    menuBar->addAction(menuGhost->menuAction());
    menuFile->addAction(actionOpenPlayer);
    menuFile->addAction(actionOpenGhost);
    menuFile->addAction(actionClosePlayer);
    menuFile->addAction(actionCloseGhost);
    menuFile->addAction(menuInputCentering->menuAction());
    menuFile->addAction(actionSwapFiles);
    menuFile->addAction(actionScrollTogether);
    menuInputCentering->addAction(action0Centered);
    menuInputCentering->addAction(action7Centered);
    menuPlayer->addAction(actionUndoPlayer);
    menuPlayer->addAction(actionRedoPlayer);
    menuGhost->addAction(actionUndoGhost);
    menuGhost->addAction(actionRedoGhost);

    setTitles();
}

void TASToolKitEditor::setTitles()
{
    setTitleNames();
    setTitleShortcuts();
}

void TASToolKitEditor::setTitleNames()
{
    setWindowTitle("TTK Input Editor");
    actionUndoPlayer->setText("Undo");
    actionRedoPlayer->setText("Redo");
    actionUndoGhost->setText("Undo");
    actionRedoGhost->setText("Redo");
    actionOpenPlayer->setText("Open Player");
    actionOpenGhost->setText("Open Ghost");
    actionClosePlayer->setText("Close Player");
    actionCloseGhost->setText("Close Ghost");
    action0Centered->setText("0 Centered");
    action7Centered->setText("7 Centered");
    actionSwapFiles->setText("Swap Player and Ghost");
    actionScrollTogether->setText("Scroll Together");
    playerLabel->setText("Player");
    ghostLabel->setText("Ghost");
    menuFile->setTitle("File");
    menuInputCentering->setTitle("Input Centering");
    menuPlayer->setTitle("Player");
    menuGhost->setTitle("Ghost");
}

void TASToolKitEditor::setTitleShortcuts()
{
#if QT_CONFIG(shortcut)
    actionUndoPlayer->setShortcut(QString("Ctrl+Z"));
    actionRedoPlayer->setShortcut(QString("Ctrl+Y"));
    actionUndoGhost->setShortcut(QString("Ctrl+Shift+Z"));
    actionRedoGhost->setShortcut(QString("Ctrl+Shift+Y"));
    actionOpenPlayer->setShortcut(QString("Ctrl+O"));
    actionOpenGhost->setShortcut(QString("Ctrl+Shift+O"));
    actionClosePlayer->setShortcut(QString("Esc"));
    actionCloseGhost->setShortcut(QString("Shift+Esc"));
#endif
}