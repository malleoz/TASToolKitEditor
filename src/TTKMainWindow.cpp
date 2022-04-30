#include "TTKMainWindow.h"

#include "InputFile.h"
#include "InputFileModel.h"

#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QTextStream>

#include <iostream>

#define FRAMECOUNT_COLUMN_WIDTH 40
#define BUTTON_COLUMN_WIDTH 20
#define STICK_COLUMN_WIDTH 25
#define PAD_COLUMN_WIDTH 35

#define COLUMN_WIDTH_SUM (FRAMECOUNT_COLUMN_WIDTH + (3 * BUTTON_COLUMN_WIDTH) + (2 * STICK_COLUMN_WIDTH) + PAD_COLUMN_WIDTH + 25)

#define TABLE_SIDE_PADDING 10
#define SINGLE_FILE_WINDOW_WIDTH ((COLUMN_WIDTH_SUM) + (2 * TABLE_SIDE_PADDING))
#define DOUBLE_FILE_WINDOW_WIDTH (SINGLE_FILE_WINDOW_WIDTH * 2)
#define DEFAULT_WINDOW_HEIGHT 500

TTKMainWindow::TTKMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    createInputFiles();
    connectActions();
}

void TTKMainWindow::createInputFiles()
{
    InputFileMenus playerMenus = InputFileMenus(menuPlayer,
                                                actionUndoPlayer,
                                                actionRedoPlayer,
                                                actionClosePlayer,
                                                action0CenteredPlayer,
                                                action7CenteredPlayer);

    InputFileMenus ghostMenus = InputFileMenus(menuGhost,
                                               actionUndoGhost,
                                               actionRedoGhost,
                                               actionCloseGhost,
                                               action0CenteredGhost,
                                               action7CenteredGhost);

    playerFile = new InputFile(playerMenus, playerLabel, playerTableView);
    ghostFile = new InputFile(ghostMenus, ghostLabel, ghostTableView);
}

void TTKMainWindow::connectActions()
{
    connect(actionOpenPlayer, &QAction::triggered, this, [this]() { openFile(playerFile); });
    connect(actionOpenGhost, &QAction::triggered, this, [this]() { openFile(ghostFile); });
    connect(actionClosePlayer, &QAction::triggered, this, [this]() { closeFile(playerFile); });
    connect(actionCloseGhost, &QAction::triggered, this, [this]() { closeFile(ghostFile); });
    connect(actionUndoPlayer, &QAction::triggered, this, [this]() { onUndoRedo(playerFile, EOperationType::Undo); });
    connect(actionUndoGhost, &QAction::triggered, this, [this]() { onUndoRedo(ghostFile, EOperationType::Undo); });
    connect(actionRedoPlayer, &QAction::triggered, this, [this]() { onUndoRedo(playerFile, EOperationType::Redo); });
    connect(actionRedoGhost, &QAction::triggered, this, [this]() { onUndoRedo(ghostFile, EOperationType::Redo); });
    connect(actionScrollTogether, &QAction::toggled, this, &TTKMainWindow::onToggleScrollTogether);
    connect(playerTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, this, [this]() { onScroll(playerFile); });
    connect(ghostTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, this, [this]() { onScroll(ghostFile); });
    connect(action0CenteredPlayer, &QAction::triggered, this, [this]() { onReCenter(playerFile, Centering::Zero); });
    connect(action0CenteredGhost, &QAction::triggered, this, [this]() { onReCenter(ghostFile, Centering::Zero); });
    connect(action7CenteredPlayer, &QAction::triggered, this, [this]() { onReCenter(playerFile, Centering::Seven); });
    connect(action7CenteredGhost, &QAction::triggered, this, [this]() { onReCenter(ghostFile, Centering::Seven); });
    connect(actionSwapFiles, &QAction::triggered, this, [this]() { playerFile->swap(ghostFile); });
}


void TTKMainWindow::openFile(FileType type)
{

}

void TTKMainWindow::closeFile(FileType type)
{

}

void TTKMainWindow::toggleCentering()
{

}

void TTKMainWindow::onScroll()
{

}




void TTKMainWindow::onReCenter(InputFile* pInputFile, Centering centering)
{
    Centering curCentering = pInputFile->getCentering();
    if (curCentering == centering || curCentering == Centering::Unknown)
        return;
    
    pInputFile->setCentering(centering);

    int stickOffset = (centering == Centering::Seven) ? 7 : -7;
    pInputFile->applyStickOffset(stickOffset);

    // So rather than have thousands of undo operations appear because of this operation,
    // just clear the stacks...
    pInputFile->getUndoStack()->clear();
    pInputFile->getRedoStack()->clear();

    // Finally, save to the file
//    InputFileModel::writeFileOnDisk(pInputFile);
}

void TTKMainWindow::onScroll(InputFile* pInputFile)
{
    if (!m_bScrollTogether)
        return;

    // What row is the current table at?
    int topRow = pInputFile->getTableView()->rowAt(0);

    // Find other table
    InputFile* otherFile;

    if (pInputFile == playerFile)
        otherFile = ghostFile;
    else
        otherFile = playerFile;

    // Scroll other table to that row
    scrollToFirstTable(pInputFile->getTableView(), otherFile->getTableView());
}

void TTKMainWindow::onToggleScrollTogether(bool bTogether)
{
    m_bScrollTogether = bTogether;

    actionScrollTogether->setChecked(m_bScrollTogether);

    if (!m_bScrollTogether)
        return;

    // Jump ghost view to same row as player view
    scrollToFirstTable(playerTableView, ghostTableView);
}

void TTKMainWindow::scrollToFirstTable(QTableView* dst, QTableView* src)
{
    int dstTopRow = dst->rowAt(0);
    QModelIndex index = src->model()->index(dstTopRow, 0);
    src->setCurrentIndex(index);
    src->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void TTKMainWindow::onUndoRedo(InputFile* pInputFile, EOperationType opType)
{
    bool bUndo = opType == EOperationType::Undo;

    InputFile::TtkStack* undoStack = pInputFile->getUndoStack();
    InputFile::TtkStack* redoStack = pInputFile->getRedoStack();

    // Refuse operation if the associated stack is empty
    if (bUndo && undoStack->count() == 0)
        return;
    if (!bUndo && redoStack->count() == 0)
        return;

    InputFile::CellEditAction action = bUndo ? undoStack->pop() : redoStack->pop();
    action.flipValues();

    if (bUndo)
        redoStack->push(action);
    else
        undoStack->push(action);

    pInputFile->setCellValue(action.row(), action.col(), action.curVal());
    emit pInputFile->getTableView()->model()->layoutChanged();

    // Adjust menu items
    pInputFile->getMenus().redo->setEnabled(redoStack->count() > 0);
    pInputFile->getMenus().undo->setEnabled(undoStack->count() > 0);

    // Move tableview to the row that was just modified
    // Determine if the row is visible on-screen right now
    int rowUpper = pInputFile->getTableView()->rowAt(0);
    int rowLower = pInputFile->getTableView()->rowAt(pInputFile->getTableView()->height());

    if (action.row() < rowUpper || action.row() > rowLower)
        pInputFile->getTableView()->scrollTo(pInputFile->getTableView()->model()->index(action.row(), 0));

//    InputFileModel::writeFileOnDisk(pInputFile);
}

void TTKMainWindow::closeFile(InputFile* pInputFile)
{
    pInputFile->closeFile();
    m_filesLoaded--;
    adjustUiOnFileClose(pInputFile);
}

bool TTKMainWindow::userClosedPreviousFile(InputFile* inputFile)
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

void TTKMainWindow::openFile(InputFile* inputFile)
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Input Files (*.csv)");

    if (filePath == "")
        return;

    if (inputFile->getPath() != "" && !userClosedPreviousFile(inputFile))
        return;

    openFile(inputFile, filePath);
}

void TTKMainWindow::openFile(InputFile* inputFile, QString filePath)
{
    if (filePath == playerFile->getPath() || filePath == ghostFile->getPath())
    {
        showError("Error Opening File", "This file is already open in the program!");
        return;
    }

    FileStatus status = inputFile->loadFile(filePath);

    if (status == FileStatus::WritePermission)
    {
        showError("Error Opening File", "This program does not have sufficient permissions to modify the file.\n\n" \
            "Try running this program in administrator mode and make sure the file is not open in another program.");
        return;
    }
    if (status == FileStatus::Parse)
    {
        QString errMsg = QString("There is an issue with the file on line %1.\n").arg(inputFile->getParseError());
        errMsg += inputFile->getParseMsg();
        showError("Error Parsing File", errMsg);
        return;
    }

    if (status != FileStatus::Success)
        return;

    m_filesLoaded++;

    adjustUiOnFileLoad(inputFile);

    connect(inputFile->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, [inputFile]{ inputFile->fileChanged(); });
    connect(inputFile->getTableView(), &QTableView::clicked, this, [inputFile](const QModelIndex& index) { inputFile->onCellClicked(index); });
}

void TTKMainWindow::adjustUiOnFileLoad(InputFile* pInputFile)
{
    adjustInputCenteringMenu(pInputFile);
    pInputFile->getMenus().root->menuAction()->setVisible(true);
    pInputFile->getMenus().close->setEnabled(true);
    pInputFile->getLabel()->setVisible(true);

    QTableView* pTable = pInputFile->getTableView();

    // TODO: fix
    TTKFileData x;

    pTable->setModel(new InputFileModel(x,Centering::Unknown,this));
    pTable->setVisible(true);

    /* This stuff really should be constant, but I can't do any of this until
    // the model is set, but I can't set the model until I instantiate the model
    // instance, but I can't instantiate the instance until I have the InputFile
    // instance, at which point I have to have the table instance already, which
    // means I can't yet give the table a model instance...
    // THIS IS SO CONFUSING!*/
    pTable->setColumnWidth(0, FRAMECOUNT_COLUMN_WIDTH);

    for (int i = 0; i < 3; i++)
        pTable->setColumnWidth(i + FRAMECOUNT_COLUMN, BUTTON_COLUMN_WIDTH);

    for (int i = 3; i < NUM_INPUT_COLUMNS - 1; i++)
        pTable->setColumnWidth(i + FRAMECOUNT_COLUMN, STICK_COLUMN_WIDTH);

    pTable->setColumnWidth(NUM_INPUT_COLUMNS - 1 + FRAMECOUNT_COLUMN, PAD_COLUMN_WIDTH);

    if (m_filesLoaded == 2)
    {
        actionSwapFiles->setEnabled(true);
        actionScrollTogether->setEnabled(true);
        setMaximumWidth(DOUBLE_FILE_WINDOW_WIDTH);
        setMinimumWidth(DOUBLE_FILE_WINDOW_WIDTH);
    }
}

void TTKMainWindow::adjustUiOnFileClose(InputFile* pInputFile)
{
    setMinimumWidth(SINGLE_FILE_WINDOW_WIDTH);
    setMaximumWidth(SINGLE_FILE_WINDOW_WIDTH);

    adjustMenuOnClose(pInputFile);
}

void TTKMainWindow::adjustInputCenteringMenu(InputFile* inputFile)
{
    Centering fileCentering = inputFile->getCentering();

    inputFile->getMenus().center7->setChecked(fileCentering == Centering::Seven);
    inputFile->getMenus().center0->setChecked(fileCentering == Centering::Zero);
}

void TTKMainWindow::adjustMenuOnClose(InputFile* inputFile)
{
    if (m_filesLoaded == 0)
    {
        inputFile->getMenus().center7->setChecked(false);
        inputFile->getMenus().center0->setChecked(false);
        actionSwapFiles->setEnabled(false);
    }
    
    m_bScrollTogether = false;
    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setChecked(false);
}

void TTKMainWindow::setTableViewSettings(QTableView* pTable)
{
    pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTable->horizontalHeader()->setMinimumSectionSize(0); // prevents minimum column size enforcement
    pTable->setVisible(false);
}

void TTKMainWindow::showError(const QString& errTitle, const QString& errMsg)
{
    QMessageBox::warning(this, errTitle, errMsg, QMessageBox::Ok);
}

void TTKMainWindow::addMenuItems()
{
    setMenuBar(menuBar = new QMenuBar(this));

    addFileMenuItems();
    addPlayerMenuItems();
    addGhostMenuItems();
}

void TTKMainWindow::addFileMenuItems()
{
    menuFile = new QMenu(menuBar);
    actionOpenPlayer = new QAction(this);
    actionOpenGhost = new QAction(this);
    actionClosePlayer = new QAction(this);
    actionClosePlayer->setEnabled(false);
    actionCloseGhost = new QAction(this);
    actionCloseGhost->setEnabled(false);
    actionSwapFiles = new QAction(this);
    actionSwapFiles->setEnabled(false);
    actionScrollTogether = new QAction(this);
    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setCheckable(true);
    actionScrollTogether->setChecked(false);
    menuFile->addAction(actionOpenPlayer);
    menuFile->addAction(actionClosePlayer);

    menuFile->addSeparator();

    menuFile->addAction(actionOpenGhost);
    menuFile->addAction(actionCloseGhost);

    menuFile->addSeparator();

    menuFile->addAction(actionSwapFiles);
    menuFile->addAction(actionScrollTogether);
    menuBar->addAction(menuFile->menuAction());
}

void TTKMainWindow::addPlayerMenuItems()
{
    menuPlayer = new QMenu(menuBar);
    menuPlayer->menuAction()->setVisible(false);
    actionUndoPlayer = new QAction(this);
    actionUndoPlayer->setEnabled(false);
    actionRedoPlayer = new QAction(this);
    actionRedoPlayer->setEnabled(false);
    action0CenteredPlayer = new QAction(this);
    action0CenteredPlayer->setCheckable(true);
    action7CenteredPlayer = new QAction(this);
    action7CenteredPlayer->setCheckable(true);
    menuCenterPlayer = new QMenu(menuFile);
    menuCenterPlayer->addAction(action0CenteredPlayer);
    menuCenterPlayer->addAction(action7CenteredPlayer);
    menuPlayer->addAction(actionUndoPlayer);
    menuPlayer->addAction(actionRedoPlayer);
    menuPlayer->addAction(menuCenterPlayer->menuAction());
    menuBar->addAction(menuPlayer->menuAction());
}

void TTKMainWindow::addGhostMenuItems()
{
    menuGhost = new QMenu(menuBar);
    menuGhost->menuAction()->setVisible(false);
    actionUndoGhost = new QAction(this);
    actionUndoGhost->setEnabled(false);
    actionRedoGhost = new QAction(this);
    actionRedoGhost->setEnabled(false);
    action0CenteredGhost = new QAction(this);
    action0CenteredGhost->setCheckable(true);
    action7CenteredGhost = new QAction(this);
    action7CenteredGhost->setCheckable(true);
    menuCenterGhost = new QMenu(menuFile);
    menuCenterGhost->addAction(action0CenteredGhost);
    menuCenterGhost->addAction(action7CenteredGhost);

    menuGhost->addAction(actionUndoGhost);
    menuGhost->addAction(actionRedoGhost);
    menuGhost->addAction(menuCenterGhost->menuAction());
    menuBar->addAction(menuGhost->menuAction());
}

void TTKMainWindow::setupUi()
{
    setMinimumWidth(SINGLE_FILE_WINDOW_WIDTH);
    setMaximumWidth(SINGLE_FILE_WINDOW_WIDTH);
    resize(SINGLE_FILE_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
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

    playerTableView = new InputTableView(horizontalLayoutWidget);
    setTableViewSettings(playerTableView);

    playerVLayout->addWidget(playerTableView);

    mainHorizLayout->addLayout(playerVLayout);

    ghostVLayout = new QVBoxLayout();
    ghostVLayout->setSpacing(6);
    ghostLabel = new QLabel(horizontalLayoutWidget);
    ghostLabel->setVisible(false);
    ghostVLayout->addWidget(ghostLabel);

    ghostTableView = new InputTableView(horizontalLayoutWidget);
    setTableViewSettings(ghostTableView);

    ghostVLayout->addWidget(ghostTableView);

    mainHorizLayout->addLayout(ghostVLayout);

    setCentralWidget(centralWidget);

    m_filesLoaded = 0;
    m_bScrollTogether = false;

    setTitles();
}

void TTKMainWindow::setTitles()
{
    setTitleNames();
    setTitleShortcuts();
}

void TTKMainWindow::setTitleNames()
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
    action0CenteredGhost->setText("0 Centered");
    action0CenteredPlayer->setText("0 Centered");
    action7CenteredGhost->setText("7 Centered");
    action7CenteredPlayer->setText("7 Centered");
    actionSwapFiles->setText("Swap Player and Ghost");
    actionScrollTogether->setText("Scroll Together");
    playerLabel->setText("Player");
    ghostLabel->setText("Ghost");
    menuFile->setTitle("File");
    menuCenterGhost->setTitle("Input Centering");
    menuCenterPlayer->setTitle("Input Centering");
    menuPlayer->setTitle("Player");
    menuGhost->setTitle("Ghost");
}

void TTKMainWindow::setTitleShortcuts()
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
