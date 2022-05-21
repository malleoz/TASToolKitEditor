#include "PlayerTypeInstance.h"

#include "data/InputFileHandler.h"
#include "data/InputFileModel.h"
#include "data/RKGFileHandler.h"
#include "data/RKGHeaderModel.h"
#include "ui/InputFileMenu.h"
#include "ui/InputTableView.h"
#include "ui/RKGDelegates.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QItemDelegate>
#include <QComboBox>

const TrackID RKGInterpreter::TRACK_ID_ORDERED_LIST[RKGHeader::TRACK_AMOUNT];

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

    setupRKGHeaderView();

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
        InputFileModel* model = dynamic_cast<InputFileModel*>(m_pTableView->model());
        model->replaceData(data, centering);
    }

    RKGHeaderModel* rkgModel = dynamic_cast<RKGHeaderModel*>(qRKGTable->model());
    rkgModel->setHeader(header);

    return loadAdjustments(centering);
}

bool PlayerTypeInstance::exportFile()
{
    const QString savePath = QFileDialog::getSaveFileName(Q_NULLPTR, "Export RKG", "", "Ghost File (*.rkg)");

    if (savePath == "")
        return false;

    RKGHeaderModel* rkgModel = dynamic_cast<RKGHeaderModel*>(qRKGTable->model());
    RKGHeader& header = rkgModel->getHeader();

    InputFileModel* model = dynamic_cast<InputFileModel*>(m_pTableView->model());
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
    disconnect(m_pMenu->getCenter7(), &QAction::triggered, dynamic_cast<InputFileModel*>(m_pTableView->model()), &InputFileModel::swapCentering);

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
    connect(m_pMenu->getCenter7(), &QAction::triggered, dynamic_cast<InputFileModel*>(m_pTableView->model()), &InputFileModel::swapCentering);

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

void PlayerTypeInstance::setupRKGHeaderView()
{
    qRKGTable = new QTableView();

    RKGHeaderModel* rkgModel = new RKGHeaderModel();
    qRKGTable->setModel(rkgModel);

    QStringList trackList;
    for (const TrackID tid : RKGInterpreter::TRACK_ID_ORDERED_LIST)
        trackList << RKGInterpreter::trackDesc(tid);

    RKGComboBoxDelegate* trackDelegate = new RKGComboBoxDelegate(trackList, qRKGTable);
    qRKGTable->setItemDelegateForRow(1, trackDelegate);


    QStringList vehicleList;
    for (int vid = 0x0; vid <= static_cast<int>(VehicleID::le_bike); vid++)
        vehicleList << RKGInterpreter::vehicleDesc(static_cast<VehicleID>(vid));

    RKGComboBoxDelegate* vehicleDelegate = new RKGComboBoxDelegate(vehicleList, qRKGTable);
    qRKGTable->setItemDelegateForRow(2, vehicleDelegate);


    QStringList characterList;
    for (int cid = 0x0; cid <= static_cast<int>(CharacterID::rs_menu); cid++)
        characterList << RKGInterpreter::characterDesc(static_cast<CharacterID>(cid));

    RKGComboBoxDelegate* characterDelegate = new RKGComboBoxDelegate(characterList, qRKGTable);
    qRKGTable->setItemDelegateForRow(3, characterDelegate);


    QStringList controllerList;
    for (int cid = 0x0; cid <= static_cast<int>(ControllerType::GCN); cid++)
        controllerList << RKGInterpreter::controllerDesc(static_cast<ControllerType>(cid));

    RKGComboBoxDelegate* controllerDelegate = new RKGComboBoxDelegate(controllerList, qRKGTable);
    qRKGTable->setItemDelegateForRow(5, controllerDelegate);


    QStringList ghostTypeList;
    for (uint8_t gid = 0x1; gid <= 0x26; gid++)
        ghostTypeList << RKGInterpreter::ghostTypeDesc(gid);

    RKGComboBoxDelegate* ghostTypeDelegate = new RKGComboBoxDelegate(ghostTypeList, qRKGTable);
    qRKGTable->setItemDelegateForRow(6, ghostTypeDelegate);


    QStringList driftTypeList = {RKGInterpreter::driftDesc(DriftType::Manual)
                                ,RKGInterpreter::driftDesc(DriftType::Automatic)};

    RKGComboBoxDelegate* driftTypeDelegate = new RKGComboBoxDelegate(driftTypeList, qRKGTable);
    qRKGTable->setItemDelegateForRow(7, driftTypeDelegate);


    QStringList lapCountList;
    for (int i = 1; i <= RKGHeader::TOTAL_LAP_TIMES; i++)
        lapCountList << QString("Lap: %1").arg(i);

    RKGComboBoxDelegate* lapCountDelegate = new RKGComboBoxDelegate(lapCountList, qRKGTable);
    qRKGTable->setItemDelegateForColumn(0, lapCountDelegate);

    adjustPersistentEditors();
    connect(rkgModel, &RKGHeaderModel::modelReset, this, &PlayerTypeInstance::adjustPersistentEditors);
}

void PlayerTypeInstance::adjustPersistentEditors()
{
    RKGHeaderModel* rkgModel = dynamic_cast<RKGHeaderModel*>(qRKGTable->model());

    qRKGTable->openPersistentEditor(rkgModel->index(1,1));
    qRKGTable->openPersistentEditor(rkgModel->index(2,1));
    qRKGTable->openPersistentEditor(rkgModel->index(3,1));
    qRKGTable->openPersistentEditor(rkgModel->index(5,1));
    qRKGTable->openPersistentEditor(rkgModel->index(6,1));
    qRKGTable->openPersistentEditor(rkgModel->index(7,1));
    qRKGTable->openPersistentEditor(rkgModel->index(RKGHeaderModel::LAPTIME_HEADER_INDEX, 0));
}
