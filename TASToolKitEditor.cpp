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
    createInputFileInstances();
    connectActions();
}

void TASToolKitEditor::createInputFileInstances()
{
    InputFileMenus playerMenus = InputFileMenus(menuPlayer, actionUndoPlayer, actionRedoPlayer, actionClosePlayer);
    InputFileMenus ghostMenus = InputFileMenus(menuGhost, actionUndoGhost, actionRedoGhost, actionCloseGhost);
    playerFile = new InputFile(playerMenus, playerLabel, playerTableView);
    ghostFile = new InputFile(ghostMenus, ghostLabel, ghostTableView);
}

void TASToolKitEditor::connectActions()
{
    connect(actionOpenPlayer, &QAction::triggered, this, &TASToolKitEditor::onOpenPlayer);
    connect(actionOpenGhost, &QAction::triggered, this, &TASToolKitEditor::onOpenGhost);
    connect(actionClosePlayer, &QAction::triggered, this, &TASToolKitEditor::onClosePlayer);
    connect(actionCloseGhost, &QAction::triggered, this, &TASToolKitEditor::onCloseGhost);
}

void TASToolKitEditor::onOpenPlayer()
{
    openFile(playerFile);
}

void TASToolKitEditor::onOpenGhost()
{
    openFile(ghostFile);
}

void TASToolKitEditor::onClosePlayer()
{
    playerFile->closeFile();
}

void TASToolKitEditor::onCloseGhost()
{
    ghostFile->closeFile();
}

bool TASToolKitEditor::userClosedPreviousFile(InputFile* inputFile)
{
    // Have user confirm they want to close file
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Close Current File", "Are you sure you want to close the current file and open a new one?",
        QMessageBox::No | QMessageBox::Yes);

    if (reply != QMessageBox::Yes)
        return false;

    inputFile->closeFile();
    return true;
}

void TASToolKitEditor::openFile(InputFile* inputFile)
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Input Files (*.csv)");

    if (inputFile->getPath() != "" && !userClosedPreviousFile(inputFile))
        return;
    
    FileStatus status = inputFile->loadFile(filePath);

    if (status != FileStatus::Success)
        return;

    m_filesLoaded++;

    adjustUiOnFileLoad(inputFile);
}

void TASToolKitEditor::adjustUiOnFileLoad(InputFile* pInputFile)
{
    adjustInputCenteringMenu(pInputFile);
    pInputFile->getMenus().root->menuAction()->setVisible(true);
    pInputFile->getMenus().close->setEnabled(true);
    pInputFile->getTableView()->setModel(new InputFileModel(pInputFile));
    pInputFile->getTableView()->setVisible(true);
    pInputFile->getLabel()->setVisible(true);

    if (m_filesLoaded == 2)
    {
        actionSwapFiles->setEnabled(true);
        resize(width() * 2, height());
    }
}

void TASToolKitEditor::adjustInputCenteringMenu(InputFile* inputFile)
{
    Centering fileCentering = inputFile->getCentering();
    action7Centered->setChecked(fileCentering == Centering::Seven);
    action0Centered->setChecked(fileCentering == Centering::Zero);
}

void TASToolKitEditor::setTableViewSettings(QTableView* pTable)
{
    pTable->setMinimumWidth(20);
    pTable->setColumnWidth(0, 40);
    for (int i = 1; i < NUM_INPUT_COLUMNS; i++)
        pTable->setColumnWidth(i, 20);

    pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTable->setVisible(false);
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
    actionClosePlayer->setEnabled(false);
    actionCloseGhost = new QAction(this);
    actionCloseGhost->setEnabled(false);
    action0Centered = new QAction(this);
    action0Centered->setCheckable(true);
    action0Centered->setEnabled(false);
    action7Centered = new QAction(this);
    action7Centered->setCheckable(true);
    action7Centered->setEnabled(false);
    menuInputCentering = new QMenu(menuFile);
    menuInputCentering->addAction(action0Centered);
    menuInputCentering->addAction(action7Centered);
    actionSwapFiles = new QAction(this);
    actionSwapFiles->setEnabled(false);
    actionScrollTogether = new QAction(this);
    actionScrollTogether->setEnabled(false);
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
    menuPlayer->menuAction()->setVisible(false);
    actionUndoPlayer = new QAction(this);
    actionRedoPlayer = new QAction(this);
    menuPlayer->addAction(actionUndoPlayer);
    menuPlayer->addAction(actionRedoPlayer);
    menuBar->addAction(menuPlayer->menuAction());
}

void TASToolKitEditor::addGhostMenuItems()
{
    menuGhost = new QMenu(menuBar);
    menuGhost->menuAction()->setVisible(false);
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
    mainHorizLayout = new QHBoxLayout(horizontalLayoutWidget);
    mainHorizLayout->setSpacing(TABLE_SIDE_PADDING);
    mainHorizLayout->setContentsMargins(11, 11, 11, 11);
   
    centralWidget->setLayout(mainHorizLayout);

    playerVLayout = new QVBoxLayout();
    playerVLayout->setSpacing(6);
    playerLabel = new QLabel(horizontalLayoutWidget);
    playerLabel->setVisible(false);
    playerVLayout->addWidget(playerLabel);

    playerTableView = new QTableView(horizontalLayoutWidget);
    setTableViewSettings(playerTableView);

    playerVLayout->addWidget(playerTableView);



    mainHorizLayout->addLayout(playerVLayout);

    ghostVLayout = new QVBoxLayout();
    ghostVLayout->setSpacing(6);
    ghostLabel = new QLabel(horizontalLayoutWidget);
    ghostLabel->setVisible(false);
    ghostVLayout->addWidget(ghostLabel);

    ghostTableView = new QTableView(horizontalLayoutWidget);

    ghostVLayout->addWidget(ghostTableView);
    setTableViewSettings(ghostTableView);


    mainHorizLayout->addLayout(ghostVLayout);

    setCentralWidget(centralWidget);

    m_filesLoaded = 0;

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