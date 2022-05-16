#include "InputFileModel.h"

#include <QBrush>
#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>


CellEditCommand::CellEditCommand(InputFileModel* pModel, const QModelIndex& index, const QString oldVal, const QString newVal)
    : m_pModel(pModel)
    , m_index(index)
    , m_oldVal(oldVal)
    , m_newVal(newVal)
{
}

void CellEditCommand::undo()
{
    m_pModel->m_fileData[m_index.row()][m_index.column() - FRAMECOUNT_COLUMN] = m_oldVal;

    m_pModel->emitDataChanged(m_index, m_index);
}

void CellEditCommand::redo()
{
    m_pModel->m_fileData[m_index.row()][m_index.column() - FRAMECOUNT_COLUMN] = m_newVal;

    m_pModel->emitDataChanged(m_index, m_index);
}


RowEditCommand::RowEditCommand(InputFileModel* pModel, const QModelIndex& topLeft, const TTKFileData& frameDataVector, const bool actionDeleted)
    : m_pModel(pModel)
    , m_topLeft(topLeft)
    , m_frameDataVector(frameDataVector)
    , m_actionDeleted(actionDeleted)
{
}

void RowEditCommand::undo()
{
    if (m_actionDeleted)
    {
        addRows();
    }
    else
    {
        removeRows();
    }

    emit m_pModel->fileToBeWritten(m_pModel->m_fileData);
}

void RowEditCommand::redo()
{
    if (m_actionDeleted)
    {
        removeRows();
    }
    else
    {
        addRows();
    }

    emit m_pModel->fileToBeWritten(m_pModel->m_fileData);
}

void RowEditCommand::addRows()
{
    const int row = m_topLeft.row();
    int i = 0;

    m_pModel->beginInsertRows(QModelIndex(), row, row + m_frameDataVector.count() - 1);
    for (FrameData t : m_frameDataVector)
    {
        m_pModel->m_fileData.insert(row + i++, t);
    }
    m_pModel->endInsertRows();
}

void RowEditCommand::removeRows()
{
    const int row = m_topLeft.row();

    m_pModel->beginRemoveRows(QModelIndex(), row, row + m_frameDataVector.count() - 1);
    m_pModel->m_fileData.remove(row, m_frameDataVector.count());
    m_pModel->endRemoveRows();
}





// ###### START OF INPUTFILEMODEL ######

InputFileModel::InputFileModel(const TTKFileData data, const Centering centering, QObject* parent)
    : QAbstractTableModel(parent)
    , m_fileData(data)
    , m_fileCentering(centering)
{
    m_undoStack.setUndoLimit(UNDO_STACK_LIMIT);
}

InputFileModel::~InputFileModel()
{
}



Qt::ItemFlags InputFileModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    switch (index.column())
    {
    case 0:
        return flags;
    case 1:
    case 2:
    case 3:
        return flags |= Qt::ItemIsUserCheckable;
    case 4:
    case 5:
    case 6:
        return flags |= Qt::ItemIsEditable;
    }

    return flags;
}

int InputFileModel::rowCount(const QModelIndex&) const
{
    return m_fileData.count();
}

int InputFileModel::columnCount(const QModelIndex&) const
{
    return NUM_INPUT_COLUMNS + FRAMECOUNT_COLUMN;
}

QVariant InputFileModel::data(const QModelIndex& index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        {
            if (index.column() == 0)
                return QString::number(index.row() + 1);
            if (index.column() < 4)
                return QVariant();

            return m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];
        }
    case Qt::CheckStateRole:
        {
            if (index.column() == 0 || index.column() > 3)
                return QVariant();

            QString value = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];
            return (value == "1") ? Qt::Checked : Qt::Unchecked;
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::BackgroundRole:
        return index.column() == 0 ? QBrush(Qt::gray) : QVariant();
    }
        
    return QVariant();
}

QVariant InputFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QString("Frame");
        case 1:
            return QString("A");
        case 2:
            return QString("B");
        case 3:
            return QString("L");
        case 4:
            return QString("LR");
        case 5:
            return QString("UD");
        case 6:
            return QString("DPad");
        }
    }

    return QVariant();
}

bool InputFileModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!checkIndex(index))
        return false;

    QString curValue = "";
    QString prevValue = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];

    if (value.toInt() == RESET_MACRO)
    {
        curValue = DefinitionUtils::GetDefaultFrameData(m_fileCentering)[index.column() - FRAMECOUNT_COLUMN];
    }
    else
    {
        switch(role)
        {
            case Qt::CheckStateRole:
            {
                if (!BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
                    return false;

                curValue = value.toInt() == Qt::Checked ? "1" : "0";

                break;
            }
            case Qt::EditRole:
            {
                if (!inputValid(index, value))
                    return false;

                if (BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
                    curValue = value.toInt() == Qt::Checked ? "1" : "0";
                else
                    curValue = QString::number(static_cast<int>(value.toFloat()));

                break;
            }
            default:
                return false;
        }
    }


    CellEditCommand* cmd = new CellEditCommand(this, index, prevValue, curValue);
    m_undoStack.push(cmd);

    return true;
}

void InputFileModel::swapCentering()
{
    if(m_fileCentering == Centering::Unknown)
        return;

    m_fileCentering = (m_fileCentering == Centering::Seven) ? Centering::Zero : Centering::Seven;
    const int stickOffset = (m_fileCentering == Centering::Seven) ? 7 : -7;

    beginResetModel();
    for (int i = 0; i < m_fileData.count(); i++)
    {
        for (int j = 3; j < 5; j++)
        {
            QString strVal = m_fileData[i][j];
            m_fileData[i][j] = QString::number(strVal.toInt() + stickOffset);
        }
    }
    endResetModel();

    emit fileToBeWritten(m_fileData);
}




bool InputFileModel::inputValid(const QModelIndex& index, const QVariant& value) const
{
    if (value == "")
        return false;

    bool bToFloat = false;
    int iValue = static_cast<int>(value.toFloat(&bToFloat));

    // Check if conversion fails (user entered invalid chars)
    if (!bToFloat)
        return false;

    if (BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
        return (iValue == 0 || iValue == 1);
    if (index.column() == DPAD_COL_IDX + FRAMECOUNT_COLUMN)
        return (iValue >= 0 && iValue <= 4);

    return DefinitionUtils::CheckCentering(m_fileCentering, iValue);
}

void InputFileModel::swap(InputFileModel* rhs)
{
    beginResetModel();
    rhs->beginResetModel();

    std::swap(m_fileData, rhs->m_fileData);
    std::swap(m_fileCentering, rhs->m_fileCentering);

    endResetModel();
    rhs->endResetModel();

    emit fileToBeWritten(m_fileData);
    emit rhs->fileToBeWritten(rhs->m_fileData);
}

bool InputFileModel::insertRows(int row, int count, const QModelIndex& parent)
{
    FrameData srcData = m_fileData[parent.row()];
    TTKFileData data;
    data.fill(srcData, count);

    RowEditCommand* cmd = new RowEditCommand(this, createIndex(row, 0), data, false);
    m_undoStack.push(cmd);


    return true;
}

bool InputFileModel::removeRows(int row, int count, const QModelIndex& parent)
{
    TTKFileData toDelete = m_fileData.mid(row, count);

    RowEditCommand* cmd = new RowEditCommand(this, createIndex(row, 0), toDelete, true);
    m_undoStack.push(cmd);


    return true;
}

void InputFileModel::copyIndices(const QModelIndexList& list, const int columnCount)
{
    int curCol = 0;
    QString copyVal;

    for (QModelIndex index : list)
    {
        curCol++;
        if (index.column() != 0)
        {
            copyVal.append(m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN]);
            if(curCol < columnCount)
                copyVal.append(',');
            else
            {
                copyVal.append('\n');
                curCol = 0;
            }
        }
    }

    copyVal.remove(copyVal.length() - 1, 1);

    QClipboard* clipBoard = QGuiApplication::clipboard();
    clipBoard->setText(copyVal);
}

// ToDo: Refactor
bool InputFileModel::pasteIndices(QModelIndexList& list, const int columnCount)
{
    // Clipboard stuff
    QClipboard* clipBoard = QGuiApplication::clipboard();
    if (!clipBoard->mimeData()->hasText())
        return false;

    const QString clipText = clipBoard->text();
    const QStringList lines = clipText.split('\n');

    // Read clipboard data
    CSVData csvData;

    for (const QString line : lines)
    {
        const QStringList frameData = line.split(',');
        if (frameData.length() == 1 && frameData[0].count() == 0)
        {
            continue;
        }
        csvData.append(frameData.toVector());
    }

    const QModelIndex startingIndex = list[0];
    const int csvColCount = csvData[0].count();

    if (csvColCount + startingIndex.column() > FRAMECOUNT_COLUMN + NUM_INPUT_COLUMNS)
        return false;

    // check index/data mass
    const int totalCSVCount = csvData.count() * csvData[0].count();
    int listCount = 0;
    for (QModelIndex index : list)
        if (index.column() != 0)
            listCount++;

    if (totalCSVCount >= listCount)
    {
        // ToDo: copy only into selection ?
        // ToDo: copy only beneath the selected row ?

        // runtime saving declarations
        const int startingRow = startingIndex.row();
        const int startingColumn = startingIndex.column() == 0 ? 1 : startingIndex.column();


        // ToDo: better index handling?
        // ToDo: check for not yet existing rows
        // check valid clipboard data
        for (int i = 0; i < csvData.count(); i++)
            for (int j = 0; j < csvColCount; j++)
                if (!inputValid(this->createIndex(startingRow + i, startingColumn + j), csvData[i][j]))
                    return false;


        // ToDo: add not existing rows
        // copy validated data
        for (int i = 0; i < csvData.count(); i++)
            for (int j = 0; j < csvColCount; j++)
            {
                const QModelIndex index = this->createIndex(startingRow + i, startingColumn + j);
                QString prevValue = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];

                CellEditCommand* cmd = new CellEditCommand(this, index, prevValue, csvData[i][j]);
                m_undoStack.push(cmd);
            }

        return true;
    }
    else
    {
        // ToDo:

    }

    return false;
}

void InputFileModel::resetData(const QModelIndexList& list)
{
    FrameData defaultData = DefinitionUtils::GetDefaultFrameData(m_fileCentering);

    beginResetModel();

    for (QModelIndex index : list)
    {
        if (index.column() == 0)
            continue;

        QString curValue = defaultData[index.column() - FRAMECOUNT_COLUMN];
        QString prevValue = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];

        CellEditCommand* cmd = new CellEditCommand(this, index, prevValue, curValue);
        m_undoStack.push(cmd);
    }

    endResetModel();
}

void InputFileModel::emitDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    emit QAbstractTableModel::dataChanged(topLeft, bottomRight);

    emit fileToBeWritten(m_fileData);
}

void InputFileModel::replaceData(const TTKFileData data, const Centering centering)
{
    beginResetModel();

    m_fileData = data;
    m_fileCentering = centering;

    endResetModel();

    m_undoStack.clear();
}
