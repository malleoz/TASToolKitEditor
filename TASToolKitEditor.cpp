#include "TASToolKitEditor.h"

#include "InputFile.h"
#include "InputFileModel.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <iostream>

#define TABLE_VIEW_WIDTH 200
#define TABLE_SIDE_PADDING 10
#define DEFAULT_WINDOW_WIDTH ((TABLE_VIEW_WIDTH) + (2 * TABLE_SIDE_PADDING))
#define DEFAULT_WINDOW_HEIGHT 500
#define DEFAULT_TABLE_COL_WIDTH 30

TASToolKitEditor::TASToolKitEditor(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    connectActions();
    createInputFileInstances();
}

void TASToolKitEditor::createInputFileInstances()
{
    playerFile = new InputFile(menuPlayer, actionUndoPlayer, actionRedoPlayer, playerTableView);
    ghostFile = new InputFile(menuGhost, actionUndoGhost, actionRedoGhost, ghostTableView);
}

void TASToolKitEditor::connectActions()
{
    connect(actionOpenPlayer, &QAction::triggered, this, &TASToolKitEditor::onOpenPlayer);
    connect(actionOpenGhost, &QAction::triggered, this, &TASToolKitEditor::onOpenGhost);
}

void TASToolKitEditor::onOpenPlayer()
{
    openFile(playerFile);
}

void TASToolKitEditor::onOpenGhost()
{
    openFile(ghostFile);
}

void TASToolKitEditor::openFile(InputFile* inputFile)
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Input Files (*.csv)");

    if (inputFile->getPath() != "")
    {
        // Have user confirm they want to close file
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Close Current File", "Are you sure you want to close the current file and open a new one?", QMessageBox::No | QMessageBox::Yes);

        if (reply != QMessageBox::Yes)
            return;

        inputFile->closeFile();
    }
    
    FileStatus status = inputFile->loadFile(filePath);

    if (status == FileStatus::Success)
    {
        Centering fileCentering = inputFile->getCentering();

        action7Centered->setChecked(fileCentering == Centering::Seven);
        action0Centered->setChecked(fileCentering == Centering::Zero);
    }

    loadDataToTableView(inputFile);
}

void TASToolKitEditor::loadDataToTableView(InputFile* inputFile)
{
    InputFileModel* pInputFileModel = new InputFileModel(inputFile);
    inputFile->getTableView()->setModel(pInputFileModel);
}

void TASToolKitEditor::showError(const QString& errTitle, const QString& errMsg)
{
    QMessageBox::warning(this, errTitle, errMsg, QMessageBox::Ok);
}

void TASToolKitEditor::addMenuItems()
{
    setMenuBar(menuBar = new QMenuBar(this));

    addFileMenuItems();
    addPlayerMenuItems();
    addGhostMenuItems();
}

void TASToolKitEditor::addFileMenuItems()
{
    menuFile = new QMenu(menuBar);
    actionOpenPlayer = new QAction(this);
    actionOpenGhost = new QAction(this);
    actionClosePlayer = new QAction(this);
    actionCloseGhost = new QAction(this);
    actionSwapFiles = new QAction(this);
    action0Centered = new QAction(this);
    action0Centered->setCheckable(true);
    action7Centered = new QAction(this);
    action7Centered->setCheckable(true);
    menuInputCentering = new QMenu(menuFile);
    menuInputCentering->addAction(action0Centered);
    menuInputCentering->addAction(action7Centered);
    actionScrollTogether = new QAction(this);

    menuFile->addAction(actionOpenPlayer);
    menuFile->addAction(actionOpenGhost);
    menuFile->addAction(actionClosePlayer);
    menuFile->addAction(actionCloseGhost);
    menuFile->addAction(menuInputCentering->menuAction());
    menuFile->addAction(actionSwapFiles);
    menuFile->addAction(actionScrollTogether);

    menuBar->addAction(menuFile->menuAction());
}

void TASToolKitEditor::addPlayerMenuItems()
{
    menuPlayer = new QMenu(menuBar);
    actionUndoPlayer = new QAction(this);
    actionRedoPlayer = new QAction(this);

    menuPlayer->addAction(actionUndoPlayer);
    menuPlayer->addAction(actionRedoPlayer);

    menuBar->addAction(menuPlayer->menuAction());
}

void TASToolKitEditor::addGhostMenuItems()
{
    menuGhost = new QMenu(menuBar);
    actionUndoGhost = new QAction(this);
    actionRedoGhost = new QAction(this);

    menuGhost->addAction(actionUndoGhost);
    menuGhost->addAction(actionRedoGhost);

    menuBar->addAction(menuGhost->menuAction());
}

void TASToolKitEditor::setupUi()
{
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    addMenuItems();

    centralWidget = new QWidget(this);
    horizontalLayoutWidget = new QWidget(centralWidget);
    horizontalLayoutWidget->setGeometry(QRect(TABLE_SIDE_PADDING, TABLE_SIDE_PADDING, (TABLE_VIEW_WIDTH * 2) + TABLE_SIDE_PADDING, DEFAULT_WINDOW_HEIGHT - (2 * TABLE_SIDE_PADDING)));
    mainHorizLayout = new QHBoxLayout(horizontalLayoutWidget);
    mainHorizLayout->setSpacing(TABLE_SIDE_PADDING);
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