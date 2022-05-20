#include "PlayerTypeInstance.h"

#include "data/InputFileHandler.h"
#include "data/InputFileModel.h"
#include "data/RKGFileHandler.h"
#include "data/RKGHeaderModel.h"
#include "ui/InputFileMenu.h"
#include "ui/InputTableView.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileSystemWatcher>


PlayerTypeInstance::PlayerTypeInstance(const PlayerType type, QObject* parent)
    : QObject(parent)
    , qVLayout(Q_NULLPTR)
    , qLabel(Q_NULLPTR)
    , qRKGTable(Q_NULLPTR)
    , m_pTableView(nullptr)
    , m_pMenu(nullptr)
    , m_pFileHandler(nullptr)
    , m_type(type)
    , m_loaded(false)
{
}

PlayerTypeInstance::~PlayerTypeInstance()
{
    delete qRKGTable;
}

void PlayerTypeInstance::setupUI(QWidget* parent)
{
    qVLayout = new QVBoxLayout(parent);
    qVLayout->setSpacing(6);

    qLabel = new QLabel();
    qLabel->setVisible(false);
    qVLayout->addWidget(qLabel);

    m_pTableView = new InputTableView();

    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pTableView->horizontalHeader()->setMinimumSectionSize(0); // prevents minimum column size enforcement
    m_pTableView->setVisible(false);

    qVLayout->addWidget(m_pTableView);

    qRKGTable = new QTableView();

    RKGHeaderModel* rkgModel = new RKGHeaderModel();
    qRKGTable->setModel(rkgModel);

    connect(m_pMenu->getClose(), &QAction::triggered, this, &PlayerTypeInstance::closeFile);
    connect(m_pMenu->getRKGHeadEdit(), &QAction::triggered, qRKGTable, &QTableView::show);
    connect(m_pMenu->getRKGExport(), &QAction::triggered, this, &PlayerTypeInstance::exportFile);
}

bool PlayerTypeInstance::openFile()
{
    const QString filePath = QFileDialog::getOpenFileName(Q_NULLPTR, "Open File", "", "Input Files (*.csv)");

    if (filePath == "")
        return m_loaded;

    if (m_pFileHandler != nullptr && !userClosedPreviousFile())
        return m_loaded;


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

        return m_loaded;
    }

    InputFileModel* model = new InputFileModel(data, centering);
    m_pTableView->setModel(model);

    return loadAdjustments(centering);
}

bool PlayerTypeInstance::importFile()
{
    const QString filePath = QFileDialog::getOpenFileName(Q_NULLPTR, "Open File", "", "Ghost File (*.rkg)");

    if (filePath == "")
        return m_loaded;

    RKGHeader header;
    TTKFileData data;
    const Centering centering = Centering::Seven;

    const FileStatus status = RKGFileHandler::loadRKGFile(filePath, header, data);

    if (status != FileStatus::Success)
    {
        return m_loaded;
    }

    if (!m_loaded)
    {
        // Make the user create a csv file for using the rkg data
        QMessageBox::StandardButton reply;

        const QString message = "To import an *.rkg ghost file, you need to write into a *.csv file. \n\n" \
                                "Do you want to want to import the ghost and save as a new *.csv file?";

        reply = QMessageBox::question(Q_NULLPTR, "RKG Import", message, QMessageBox::No | QMessageBox::Yes);

        if (reply != QMessageBox::Yes)
            return m_loaded;

        const QString savePath = QFileDialog::getSaveFileName(Q_NULLPTR, "Create File", "", "Input Files (*.csv)");

        if (savePath == "")
            return m_loaded;

        m_pFileHandler = new InputFileHandler(savePath);

        InputFileModel* model = new InputFileModel(data, centering);
        m_pTableView->setModel(model);

        m_pFileHandler->saveFile(data);
    }
    else
    {
        InputFileModel* model = reinterpret_cast<InputFileModel*>(m_pTableView->model());
        model->replaceData(data, centering);
    }

    RKGHeaderModel* rkgModel = reinterpret_cast<RKGHeaderModel*>(qRKGTable->model());
    rkgModel->setHeader(header);

    return loadAdjustments(centering);
}

bool PlayerTypeInstance::exportFile()
{
    const QString savePath = QFileDialog::getSaveFileName(Q_NULLPTR, "Export RKG", "", "Ghost File (*.rkg)");

    if (savePath == "")
        return false;

    RKGHeaderModel* rkgModel = reinterpret_cast<RKGHeaderModel*>(qRKGTable->model());
    RKGHeader& header = rkgModel->getHeader();

    InputFileModel* model = reinterpret_cast<InputFileModel*>(m_pTableView->model());
    const TTKFileData& data = model->getData();

    RKGFileHandler::saveRKGFile(savePath, header, data);

    return true;
}

void PlayerTypeInstance::reloadFile()
{
    TTKFileData data;
    Centering centering;
    if (m_pFileHandler->loadFile(data, centering) != FileStatus::Success)
    {
        closeFile();

        return;
    }

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

    disconnect(pModel->getUndoStack(), &QUndoStack::canUndoChanged, m_pMenu->getUndo(), &QAction::setEnabled);
    disconnect(pModel->getUndoStack(), &QUndoStack::canRedoChanged, m_pMenu->getRedo(), &QAction::setEnabled);

    disconnect(m_pFileHandler->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, &PlayerTypeInstance::reloadFile);

    // delete

    delete m_pFileHandler;
    m_pFileHandler = nullptr;

    m_pTableView->setModel(nullptr);
    delete pModel;

    m_loaded = false;
    adjustUiOnFileClose();

    emit fileClosed();
}



bool PlayerTypeInstance::loadAdjustments(const Centering centering)
{
    adjustUiOnFileLoad(centering);

    // connecting actions

    InputFileModel* pModel = reinterpret_cast<InputFileModel*>(m_pTableView->model());

    connect(pModel, &InputFileModel::fileToBeWritten, m_pFileHandler, &InputFileHandler::saveFile);
    connect(m_pMenu->getCenter7(), &QAction::triggered, reinterpret_cast<InputFileModel*>(m_pTableView->model()), &InputFileModel::swapCentering);

    connect(m_pMenu->getUndo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::undo);
    connect(m_pMenu->getRedo(), &QAction::triggered, pModel->getUndoStack(), &QUndoStack::redo);

    connect(pModel->getUndoStack(), &QUndoStack::canUndoChanged, m_pMenu->getUndo(), &QAction::setEnabled);
    connect(pModel->getUndoStack(), &QUndoStack::canRedoChanged, m_pMenu->getRedo(), &QAction::setEnabled);

    connect(m_pFileHandler->getFsWatcher(), &QFileSystemWatcher::fileChanged, this, &PlayerTypeInstance::reloadFile);

    return m_loaded = true;
}

bool PlayerTypeInstance::userClosedPreviousFile()
{
    if (m_pFileHandler == nullptr)
        return true;

    // Have user confirm they want to close file
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(Q_NULLPTR, "Close Current File", "Are you sure you want to close the current file and open a new one?",
        QMessageBox::No | QMessageBox::Yes);

    if (reply != QMessageBox::Yes)
        return false;

    closeFile();

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
