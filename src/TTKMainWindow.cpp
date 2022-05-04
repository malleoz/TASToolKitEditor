#include "TTKMainWindow.h"

#include "InputFile.h"
#include "InputFileMenu.h"
#include "InputFileModel.h"

#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>

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
    , m_player(PlayerType::Player)
    , m_ghost(PlayerType::Ghost)
    , m_bScrollTogether(false)
{
    setupUi();
    connectActions();
}


void TTKMainWindow::openFile(PlayerTypeInstance& typeInstance)
{
    typeInstance.openFile(this);

    if (amountLoadedFiles() == 2)
    {
        actionSwapFiles->setEnabled(true);
        actionScrollTogether->setEnabled(true);
        setMaximumWidth(DOUBLE_FILE_WINDOW_WIDTH);
        setMinimumWidth(DOUBLE_FILE_WINDOW_WIDTH);
    }

    // ToDo: connect ?
//    connect(inputFile->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, [inputFile]{ inputFile->fileChanged(); });
//    connect(inputFile->getTableView(), &QTableView::clicked, this, [inputFile](const QModelIndex& index) { inputFile->onCellClicked(index); });

}

void TTKMainWindow::closeFile(PlayerTypeInstance& typeInstance)
{
    typeInstance.closeFile();

    setMinimumWidth(SINGLE_FILE_WINDOW_WIDTH);
    setMaximumWidth(SINGLE_FILE_WINDOW_WIDTH);

    actionSwapFiles->setEnabled(false);

    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setChecked(false);
    m_bScrollTogether = false;
}

void TTKMainWindow::onScroll()
{

}

void TTKMainWindow::onToggleScrollTogether(bool bTogether)
{
    m_bScrollTogether = bTogether;

    actionScrollTogether->setChecked(m_bScrollTogether);

    if (!m_bScrollTogether)
        return;

    // Jump ghost view to same row as player view
    scrollToFirstTable(m_player.getTableView(), m_ghost.getTableView());
}

void TTKMainWindow::swapModels()
{
    InputFileModel* playerModel = reinterpret_cast<InputFileModel*>(m_player.getTableView()->model());
    InputFileModel* ghostModel = reinterpret_cast<InputFileModel*>(m_ghost.getTableView()->model());

    playerModel->swap(ghostModel);
}

uint8_t TTKMainWindow::amountLoadedFiles()
{
    uint8_t amountLoaded = 0;

    if (m_player.isLoaded())
        amountLoaded++;
    if (m_ghost.isLoaded())
        amountLoaded++;

    return amountLoaded;
}


void TTKMainWindow::connectActions()
{
    connect(actionOpenPlayer, &QAction::triggered, this, [this]() { openFile(m_player);} );
    connect(actionOpenGhost, &QAction::triggered, this, [this]() { openFile(m_ghost);} );

    connect(m_player.getMenu()->getClose(), &QAction::triggered, this, [this]() { closeFile(m_player); });
    connect(m_ghost.getMenu()->getClose(), &QAction::triggered, this, [this]() { closeFile(m_ghost); });


    InputFileModel* pPlayerModel = reinterpret_cast<InputFileModel*>(m_player.getTableView()->model());
    InputFileModel* pGhostModel = reinterpret_cast<InputFileModel*>(m_ghost.getTableView()->model());

    connect(m_player.getMenu()->getUndo(), &QAction::triggered, this, [&]() { pPlayerModel->undo(); });
    connect(m_ghost.getMenu()->getUndo(), &QAction::triggered, this, [&]() { pGhostModel->undo(); });

    connect(m_player.getMenu()->getRedo(), &QAction::triggered, this, [&]() { pPlayerModel->redo(); });
    connect(m_ghost.getMenu()->getRedo(), &QAction::triggered, this, [&]() { pGhostModel->redo(); });


    connect(actionScrollTogether, &QAction::toggled, this, &TTKMainWindow::onToggleScrollTogether);

    connect(actionSwapFiles, &QAction::triggered, this, [this]() { swapModels(); });


    // TODO: check connecting and disconnecting siagnals for scrolling
    // both scrollbars at the same time

//    connect(m_pPlayerTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, this, [this]() { onScroll(playerFile); });
//    connect(m_pGhostTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, this, [this]() { onScroll(ghostFile); });
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

void TTKMainWindow::scrollToFirstTable(InputTableView* dst, InputTableView* src)
{
    int dstTopRow = dst->rowAt(0);
    QModelIndex index = src->model()->index(dstTopRow, 0);
    src->setCurrentIndex(index);
    src->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void TTKMainWindow::adjustInputCenteringMenu(InputFile* inputFile)
{
    Centering fileCentering = inputFile->getCentering();

    if (fileCentering == Centering::Unknown)
    {
        inputFile->getMenus()->getCenter7()->setChecked(false);
        inputFile->getMenus()->getCenter7()->setEnabled(false);
        return;
    }

    inputFile->getMenus()->getCenter7()->setEnabled(true);
    inputFile->getMenus()->getCenter7()->setChecked(fileCentering == Centering::Seven);
}

void TTKMainWindow::addMenuItems()
{
    setMenuBar(menuBar = new QMenuBar(this));

    addFileMenuItems();

    InputFileMenu* playerMenu = new InputFileMenu("Player", "Ctrl+", menuBar);
    InputFileMenu* ghostMenu = new InputFileMenu("Ghost", "Ctrl+Shift+", menuBar);

    m_player.setMenu(playerMenu);
    m_ghost.setMenu(ghostMenu);

    menuBar->addAction(playerMenu->menuAction());
    menuBar->addAction(ghostMenu->menuAction());
}

void TTKMainWindow::addFileMenuItems()
{
    menuFile = new QMenu(menuBar);

    actionOpenPlayer = new QAction(this);
    actionOpenGhost = new QAction(this);
    actionSwapFiles = new QAction(this);
    actionSwapFiles->setEnabled(false);

    actionScrollTogether = new QAction(this);
    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setCheckable(true);
    actionScrollTogether->setChecked(false);

    menuFile->addAction(actionOpenPlayer);

    menuFile->addAction(actionOpenGhost);
    menuFile->addSeparator();

    menuFile->addAction(actionSwapFiles);
    menuFile->addAction(actionScrollTogether);
    menuBar->addAction(menuFile->menuAction());
}

void TTKMainWindow::setupUi()
{
    // setup main window
    setMinimumWidth(SINGLE_FILE_WINDOW_WIDTH);
    setMaximumWidth(SINGLE_FILE_WINDOW_WIDTH);
    resize(SINGLE_FILE_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
    addMenuItems();


    // creating main widgets
    centralWidget = new QWidget(this);
    
    horizontalLayoutWidget = new QWidget(centralWidget);
    mainHorizLayout = new QHBoxLayout(horizontalLayoutWidget);
    mainHorizLayout->setSpacing(TABLE_SIDE_PADDING);
    mainHorizLayout->setContentsMargins(11, 11, 11, 11);
   
    centralWidget->setLayout(mainHorizLayout);

    // creating player/ghost layouts
    m_player.setupUI();
    mainHorizLayout->addLayout(m_player.getLayout());

    m_ghost.setupUI();
    mainHorizLayout->addLayout(m_ghost.getLayout());


    // misc.
    setCentralWidget(centralWidget);

    setTitles();
}

void TTKMainWindow::setTitles()
{
    setWindowTitle("TTK Input Editor");

    menuFile->setTitle("File");

    actionOpenPlayer->setText("Open Player");
    actionOpenGhost->setText("Open Ghost");
    actionSwapFiles->setText("Swap Player and Ghost");
    actionScrollTogether->setText("Scroll Together");

    m_player.getLabel()->setText("Player");
    m_ghost.getLabel()->setText("Ghost");

#if QT_CONFIG(shortcut)
    actionOpenPlayer->setShortcut(QString("Ctrl+O"));
    actionOpenGhost->setShortcut(QString("Ctrl+Shift+O"));
#endif
}
