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

bool InputFileModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, row + count - 1);
    FrameData data = m_fileData[row];
    m_fileData.insert(row, count, data);
    endInsertRows();

    return true;
}

bool InputFileModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!checkIndex(index))
        return false;

    QString prevValue = m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN];
    QString curValue = "";

    if (role != Qt::EditRole)
        return false;

    if (!inputValid(index, value))
        return false;

    if (BUTTON_COL_IDXS.contains(index.column() - FRAMECOUNT_COLUMN))
    {
        curValue = value.toInt() == Qt::Checked ? "1" : "0";
    }
    else
    {
        curValue = QString::number(static_cast<int>(value.toFloat()));
    }

    m_fileData[index.row()][index.column() - FRAMECOUNT_COLUMN] = curValue;

    return true;


//    setCachedFileData(index.row(), index.column() - FRAMECOUNT_COLUMN, curValue);
//    addToStack(InputFile::CellEditAction(index.row(), index.column() - FRAMECOUNT_COLUMN, prevValue, curValue));
//    writeFileOnDisk(m_pFile);

//    m_pFile->getTableView()->viewport()->update();

//    return false;
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
