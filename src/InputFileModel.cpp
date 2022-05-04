#include "InputFileModel.h"

#include <iostream>
#include <fstream>

#include <QAction>
#include <QBrush>
#include <QTableView>

#define UNDO_STACK_LIMIT 100



InputFileModel::CellEditAction::CellEditAction(int row, int col, QString prev, QString cur)
    : m_rowIdx(row)
    , m_colIdx(col)
    , m_prev(prev)
    , m_cur(cur)
{
}

bool InputFileModel::CellEditAction::operator==(const CellEditAction& rhs)
{
    return m_rowIdx == rhs.m_rowIdx && m_colIdx == rhs.m_colIdx && m_cur == rhs.m_cur;
}


InputFileModel::InputFileModel(const TTKFileData data, const Centering centering, QObject* parent)
    : QAbstractTableModel(parent)
    , m_fileData(data)
    , m_fileCentering(centering)
{
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

    QString prevValue = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];

    if (role != Qt::CheckStateRole && role != Qt::EditRole)
        return false;

    QString curValue = "";

    if (role == Qt::CheckStateRole)
    {
        if (!BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
            return false;

        curValue = value.toInt() == Qt::Checked ? "1" : "0";
    }
    else if (role == Qt::EditRole)
    {
        if (!inputValid(index, value))
            return false;

        if (BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
            curValue = value.toInt() == Qt::Checked ? "1" : "0";
        else
            curValue = QString::number(static_cast<int>(value.toFloat()));
    }

    m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN] = curValue;

    addActionToStack(CellEditAction(index.row(), index.column() - FRAMECOUNT_COLUMN, prevValue, curValue));

//    m_pFileHandler->saveFile(m_fileData);
    emit dataChanged(m_fileData);

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

    emit dataChanged(m_fileData);
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

void InputFileModel::addActionToStack(CellEditAction action)
{
    if (redoStack.count() > 0)
    {
        CellEditAction redoTop = redoStack.top();
        redoTop.flipValues();

        // If user performs same action as in top of redo stack, act as if it's a redo
        // Otherwise, just clear the redo stack since we've changed timelines
        if (action == redoTop)
            redoStack.pop();
        else
            redoStack.clear();

        undoStack.push(action);
    }
    else
    {
        undoStack.push(action);

        // Limit stack size
        if (undoStack.count() > UNDO_STACK_LIMIT)
            undoStack.pop_front();
    }

    // Update UI somehow. Emit signal?
}

void InputFileModel::undo()
{
    // Nothing to undo if stack is empty
    if (undoStack.count() == 0)
        return;

    CellEditAction action = undoStack.pop();
    QModelIndex editIndex = index(action.m_rowIdx, action.m_colIdx);
    setData(editIndex, action.m_cur, Qt::EditRole);

    action.flipValues();
    redoStack.push(action);
}

void InputFileModel::redo()
{
    // Nothing to redo if stack is empty
    if (redoStack.count() == 0)
        return;

    CellEditAction action = redoStack.pop();
    QModelIndex editIndex = index(action.m_rowIdx, action.m_colIdx);
    setData(editIndex, action.m_cur, Qt::EditRole);

    action.flipValues();
    undoStack.push(action);
}

void InputFileModel::swap(InputFileModel* rhs)
{
    beginResetModel();
    rhs->beginResetModel();

    std::swap(m_fileData, rhs->m_fileData);
    std::swap(m_fileCentering, rhs->m_fileCentering);

    endResetModel();
    rhs->endResetModel();

    emit dataChanged(m_fileData);
    emit rhs->dataChanged(rhs->m_fileData);
}

bool InputFileModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginResetModel();

    FrameData srcData = m_fileData[parent.row()];
    int frameCount = m_fileData[row][0].toInt();

    for (int i = 0; i < count; i++)
    {
        // Compute framecount
        srcData[0] = ++frameCount;

        m_fileData.insert(row, srcData);

        // Determine how to store in undo/redo stack later
    }

    QModelIndex topLeftIndex = index(row, 0);
    QModelIndex bottomRightIndex = index(row + count, NUM_INPUT_COLUMNS + FRAMECOUNT_COLUMN);
    
    endResetModel();
    
    emit dataChanged(m_fileData);

    return true;
}
