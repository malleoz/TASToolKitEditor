#include "TTKMainWindow.h"

#include "ui/InputFileMenu.h"
#include "ui/InputTableView.h"
#include "data/InputFileModel.h"

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
{
    setupUi();
    connectActions();
}


void TTKMainWindow::openFile(PlayerTypeInstance& typeInstance)
{
    if (!typeInstance.openFile())
        adjustUiOnClose();
    else
        adjustUiOnOpen();
}

void TTKMainWindow::importFile(PlayerTypeInstance& typeInstance)
{
    if (!typeInstance.importFile())
        adjustUiOnClose();
    else
        adjustUiOnOpen();
}

void TTKMainWindow::onToggleScrollTogether(bool bTogether)
{
    QScrollBar* pPlayerScrollBar = m_player.getTableView()->verticalScrollBar();
    QScrollBar* pGhostScrollBar = m_ghost.getTableView()->verticalScrollBar();


    if (bTogether)
    {
        pGhostScrollBar->setValue(pPlayerScrollBar->value());

        connect(pPlayerScrollBar, &QAbstractSlider::valueChanged, pGhostScrollBar, &QAbstractSlider::setValue);
        connect(pGhostScrollBar, &QAbstractSlider::valueChanged, pPlayerScrollBar, &QAbstractSlider::setValue);
    }
    else
    {
        disconnect(pPlayerScrollBar, &QAbstractSlider::valueChanged, pGhostScrollBar, &QAbstractSlider::setValue);
        disconnect(pGhostScrollBar, &QAbstractSlider::valueChanged, pPlayerScrollBar, &QAbstractSlider::setValue);
    }
}

void TTKMainWindow::swapModels()
{
    InputFileModel* playerModel = dynamic_cast<InputFileModel*>(m_player.getTableView()->model());
    InputFileModel* ghostModel = dynamic_cast<InputFileModel*>(m_ghost.getTableView()->model());

    playerModel->swap(ghostModel);
}

void TTKMainWindow::adjustUiOnClose()
{
    setMinimumWidth(SINGLE_FILE_WINDOW_WIDTH);
    setMaximumWidth(SINGLE_FILE_WINDOW_WIDTH);

    actionSwapFiles->setEnabled(false);

    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setChecked(false);
}

void TTKMainWindow::adjustUiOnOpen()
{
    if (bothFilesLoaded())
    {
        actionSwapFiles->setEnabled(true);
        actionScrollTogether->setEnabled(true);
        setMaximumWidth(DOUBLE_FILE_WINDOW_WIDTH);
        setMinimumWidth(DOUBLE_FILE_WINDOW_WIDTH);
    }
}

bool TTKMainWindow::bothFilesLoaded()
{
    return m_player.isLoaded() && m_ghost.isLoaded();
}


void TTKMainWindow::connectActions()
{
    connect(actionOpenPlayer, &QAction::triggered, this, [this]() { openFile(m_player);} );
    connect(actionImportPlayerRKG, &QAction::triggered, this, [this]() { importFile(m_player);} );

    connect(actionOpenGhost, &QAction::triggered, this, [this]() { openFile(m_ghost);} );
    connect(actionImportGhostRKG, &QAction::triggered, this, [this]() { importFile(m_ghost);} );

    connect(&m_player, &PlayerTypeInstance::fileClosed, this, &TTKMainWindow::adjustUiOnClose);
    connect(&m_ghost, &PlayerTypeInstance::fileClosed, this, &TTKMainWindow::adjustUiOnClose);

    connect(actionScrollTogether, &QAction::toggled, this, &TTKMainWindow::onToggleScrollTogether);

    connect(actionSwapFiles, &QAction::triggered, this, &TTKMainWindow::swapModels);
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
    actionImportPlayerRKG = new QAction(this);
    actionOpenGhost = new QAction(this);
    actionImportGhostRKG = new QAction(this);

    actionSwapFiles = new QAction(this);
    actionSwapFiles->setEnabled(false);

    actionScrollTogether = new QAction(this);
    actionScrollTogether->setEnabled(false);
    actionScrollTogether->setCheckable(true);
    actionScrollTogether->setChecked(false);

    menuFile->addAction(actionOpenPlayer);
    menuFile->addAction(actionImportPlayerRKG);
    menuFile->addSeparator();

    menuFile->addAction(actionOpenGhost);
    menuFile->addAction(actionImportGhostRKG);
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
    actionImportPlayerRKG->setText("Import Player RKG");

    actionOpenGhost->setText("Open Ghost");
    actionImportGhostRKG->setText("Import Ghost RKG");

    actionSwapFiles->setText("Swap Player and Ghost");
    actionScrollTogether->setText("Scroll Together");

    m_player.getLabel()->setText("Player");
    m_ghost.getLabel()->setText("Ghost");

#if QT_CONFIG(shortcut)
    actionOpenPlayer->setShortcut(QString("Ctrl+O"));
    actionOpenGhost->setShortcut(QString("Ctrl+Shift+O"));
#endif
}
