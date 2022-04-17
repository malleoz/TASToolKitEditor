#include "TASToolKitEditor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <iostream>

#define TABLE_VIEW_WIDTH 200
#define TABLE_SIDE_PADDING 10
#define DEFAULT_WINDOW_WIDTH ((TABLE_VIEW_WIDTH) + (2 * TABLE_SIDE_PADDING))
#define DEFAULT_WINDOW_HEIGHT 500

#define INVALID_IDX -1

#define NUM_INPUT_COLUMNS 6

/*
 * TODO
 * - figure out QTableView and how to specify vector as source
 */

TASToolKitEditor::TASToolKitEditor(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    connectActions();
    createInputFileInstances();
}

void TASToolKitEditor::createInputFileInstances()
{
    playerFile = new InputFile(menuPlayer, actionUndoPlayer, actionRedoPlayer);
    ghostFile = new InputFile(menuGhost, actionUndoGhost, actionRedoGhost);
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

    menuInputCentering = new QMenu(menuFile);
    menuInputCentering->addAction(action0Centered);
    menuInputCentering->addAction(action7Centered);

    action0Centered = new QAction(this);
    action7Centered = new QAction(this);
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

InputFile::InputFile(QMenu* root, QAction* undo, QAction* redo)
    : m_filePath("")
    , m_fileCentering(Centering::Unknown)
    , m_tableViewLoaded(false)
    , pTableView(nullptr)
    , pRootMenu(root)
    , pUndoMenu(undo)
    , pRedoMenu(redo)
    , m_frameParseError(INVALID_IDX)
{
}

FileStatus InputFile::loadFile(QString path)
{
    m_filePath = path;

    QFile fp(m_filePath);
    if (!fp.open(QIODevice::ReadWrite))
        return FileStatus::WritePermission;

    QTextStream ts(&fp);

    while (!ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList frameData = line.split(',');

        if (!valuesFormattedProperly(frameData))
        {
            m_frameParseError = m_fileData.count();
            clearData();
            return FileStatus::Parse;
        }
    }
}

void InputFile::clearData()
{
    m_filePath = "";
    m_fileData.clear();
}

bool InputFile::valuesFormattedProperly(const QStringList& data)
{
    // There should be 6 comma-separated values per line
    if (data.count() != NUM_INPUT_COLUMNS)
        return false;

    // Certain columns have restricted values
    if (!valueRestrictionsAreMet(data))
        return false;

    // Place other error checks here

    return true;
}

bool InputFile::valueRestrictionsAreMet(const QStringList& data)
{
    for (int i = 0; i < data.count(); i++)
    {
        bool ret;
        int value = data[i].toInt(&ret);

        if (!ret)
            return false;

        int smallestAcceptedVal = getSmallestAcceptedValue(i, value);
        int largestAcceptedVal = getLargestAcceptedValue(i, value);

        if (value > largestAcceptedVal || value < smallestAcceptedVal)
            return false;
    }

    return true;
}

int InputFile::getSmallestAcceptedValue(int index, int value)
{
    if (BUTTON_COL_IDXS.contains(index) || DPAD_COL_IDX == index)
        return 0;

    if (m_fileCentering == Centering::Unknown && !ableToDiscernCentering(value))
        return 0;

    return m_fileCentering == Centering::Seven ? 0 : -7;
}

int InputFile::getLargestAcceptedValue(int index, int value)
{
    if (DPAD_COL_IDX == index)
        return 4;
    if (BUTTON_COL_IDXS.contains(index))
        return 1;
    if (m_fileCentering == Centering::Unknown && !ableToDiscernCentering(value))
        return 7;

    return m_fileCentering == Centering::Seven ? 14 : 7;
}

bool InputFile::ableToDiscernCentering(int value)
{
    if (value > 7)
        m_fileCentering = Centering::Seven;
    else if (value < 0)
        m_fileCentering = Centering::Zero;
    else
        return false;

    return true;
}


void InputFile::closeFile()
{

}

CellEditAction::CellEditAction()
    : m_rowIdx(INVALID_IDX)
    , m_colIdx(INVALID_IDX)
    , m_prev(0)
    , m_cur(0)
{
}

bool CellEditAction::operator==(const CellEditAction& rhs)
{
    return m_rowIdx == rhs.m_rowIdx && m_colIdx == rhs.m_colIdx && m_cur == rhs.m_cur;
}

void CellEditAction::flipValues()
{
    int temp = m_cur;
    m_cur = m_prev;
    m_prev = temp;
}