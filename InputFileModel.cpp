#include "InputFileModel.h"

InputFileModel::InputFileModel(InputFile* pFile, QObject* parent)
    : QAbstractTableModel(parent)
    , m_pFile(pFile)
{
}

int InputFileModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_pFile->getData().count();
}

int InputFileModel::columnCount(const QModelIndex& /*parent*/) const
{
    return NUM_INPUT_COLUMNS + FRAMECOUNT_COLUMN;
}

QVariant InputFileModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return QString::number(index.row() + 1);

        return m_pFile->getCellValue(index.row(), index.column() - FRAMECOUNT_COLUMN);
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
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;

        m_pFile->setCellValue(index.row(), index.column(), value.toString());
        return true;
    }

    return false;
}
