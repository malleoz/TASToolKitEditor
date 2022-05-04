#include "PlayerTypeInstance.h"

#include "InputFileMenu.h"
#include "InputTableView.h"
#include "InputFile.h"
#include "InputFileModel.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileSystemWatcher>


#define FRAMECOUNT_COLUMN_WIDTH 40
#define BUTTON_COLUMN_WIDTH 20
#define STICK_COLUMN_WIDTH 25
#define PAD_COLUMN_WIDTH 35


PlayerTypeInstance::PlayerTypeInstance(const PlayerType type, QObject* parent)
    : QObject(parent)
    , qVLayout(Q_NULLPTR)
    , qLabel(Q_NULLPTR)
    , m_pTableView(nullptr)
    , m_pMenu(nullptr)
    , m_pFileHandler(nullptr)
    , m_type(type)
    , m_loaded(false)
{
}

void PlayerTypeInstance::setupUI(QWidget* parent)
{
    qVLayout = new QVBoxLayout(parent);
    qVLayout->setSpacing(6);

    qLabel = new QLabel();
    qLabel->setVisible(false);
    qVLayout->addWidget(qLabel);

    m_pTableView = new InputTableView(qLabel);

    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pTableView->horizontalHeader()->setMinimumSectionSize(0); // prevents minimum column size enforcement
    m_pTableView->setVisible(false);

    qVLayout->addWidget(m_pTableView);
}

void PlayerTypeInstance::openFile(QWidget* main)
{
    const QString filePath = QFileDialog::getOpenFileName(main, "Open File", "", "Input Files (*.csv)");

    if (filePath == "")
        return;

    if (m_pFileHandler != nullptr && !userClosedPreviousFile(main))
        return;


    m_pFileHandler = new InputFileHandler(filePath);
    TTKFileData data;
    Centering centering;

    const FileStatus status = m_pFileHandler->loadFile(data, centering);
    if (status != FileStatus::Success)
    {
        delete (m_pFileHandler);
        m_pFileHandler = nullptr;

        m_loaded = false;
        // Error Handling

        return;
    }

    InputFileModel* model = new InputFileModel(data, centering);
    m_pTableView->setModel(model);

    m_loaded = true;
    adjustUiOnFileLoad(centering);


    // ToDo: connect ?
//    connect(inputFile->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, [inputFile]{ inputFile->fileChanged(); });
//    connect(inputFile->getTableView(), &QTableView::clicked, this, [inputFile](const QModelIndex& index) { inputFile->onCellClicked(index); });

    InputFileModel* pModel = reinterpret_cast<InputFileModel*>(m_pTableView->model());

    connect(pModel, &InputFileModel::fileToBeWritten, this, &PlayerTypeInstance::saveFile);
    connect(m_pMenu->getCenter7(), &QAction::triggered, reinterpret_cast<InputFileModel*>(m_pTableView->model()), &InputFileModel::swapCentering);
    
    connect(m_pMenu->getUndo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::undo);
    connect(m_pMenu->getRedo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::redo);

    connect(pModel->getUndoStack(), &QUndoStack::canUndoChanged, m_pMenu->getUndo(), &QAction::setEnabled);
    connect(pModel->getUndoStack(), &QUndoStack::canRedoChanged, m_pMenu->getRedo(), &QAction::setEnabled);

    connect(m_pFileHandler->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, &PlayerTypeInstance::reloadFile);
}

void PlayerTypeInstance::saveFile(const TTKFileData& data)
{
    // Remove path from filesystem watcher so that fileChanged is not emitted when we save the file
    QFileSystemWatcher* pFsWatcher = m_pFileHandler->getFsWatcher();
    const QString path = m_pFileHandler->getPath();
    
    pFsWatcher->removePath(path);

    m_pFileHandler->saveFile(data);

    pFsWatcher->addPath(path);
}

void PlayerTypeInstance::reloadFile()
{
    TTKFileData data;
    Centering centering;
    m_pFileHandler->loadFile(data, centering);
    InputFileModel* pModel = reinterpret_cast<InputFileModel*>(m_pTableView->model());
    pModel->replaceData(data, centering);

    if (centering == Centering::Seven)
        m_pMenu->setCenter7(true);
    else
        m_pMenu->setCenter7(false);
}

void PlayerTypeInstance::closeFile()
{
    // disconnect
    InputFileModel* pModel = reinterpret_cast<InputFileModel*>(m_pTableView->model());

    disconnect(pModel, &InputFileModel::fileToBeWritten, m_pFileHandler, &InputFileHandler::saveFile);
    disconnect(m_pMenu->getCenter7(), &QAction::triggered, reinterpret_cast<InputFileModel*>(m_pTableView->model()), &InputFileModel::swapCentering);

    disconnect(m_pMenu->getUndo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::undo);
    disconnect(m_pMenu->getRedo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::redo);

    // delete

    delete m_pFileHandler;
    m_pFileHandler = nullptr;

    m_pTableView->setModel(nullptr);
    delete pModel;

    m_loaded = false;
    adjustUiOnFileClose();
}


bool PlayerTypeInstance::userClosedPreviousFile(QWidget* main)
{
    if (m_pFileHandler == nullptr)
        return true;

    // Have user confirm they want to close file
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(main, "Close Current File", "Are you sure you want to close the current file and open a new one?",
        QMessageBox::No | QMessageBox::Yes);

    if (reply != QMessageBox::Yes)
        return false;

    delete (m_pFileHandler);
    m_pFileHandler = nullptr;

    return true;
}

void PlayerTypeInstance::adjustUiOnFileLoad(const Centering centering)
{
    m_pMenu->menuAction()->setVisible(true);
    qLabel->setVisible(true);

    m_pTableView->setVisible(true);

    if (centering == Centering::Seven)
        m_pMenu->setCenter7(true);
    else
        m_pMenu->setCenter7(false);

    /* This stuff really should be constant, but I can't do any of this until
    // the model is set, but I can't set the model until I instantiate the model
    // instance, but I can't instantiate the instance until I have the InputFile
    // instance, at which point I have to have the table instance already, which
    // means I can't yet give the table a model instance...
    // THIS IS SO CONFUSING!*/
    m_pTableView->setColumnWidth(0, FRAMECOUNT_COLUMN_WIDTH);

    for (int i = 0; i < 3; i++)
        m_pTableView->setColumnWidth(i + FRAMECOUNT_COLUMN, BUTTON_COLUMN_WIDTH);

    for (int i = 3; i < NUM_INPUT_COLUMNS - 1; i++)
        m_pTableView->setColumnWidth(i + FRAMECOUNT_COLUMN, STICK_COLUMN_WIDTH);

    m_pTableView->setColumnWidth(NUM_INPUT_COLUMNS - 1 + FRAMECOUNT_COLUMN, PAD_COLUMN_WIDTH);
}

void PlayerTypeInstance::adjustUiOnFileClose()
{
    m_pMenu->menuAction()->setVisible(false);
    qLabel->setVisible(false);

    m_pTableView->setVisible(false);
}
