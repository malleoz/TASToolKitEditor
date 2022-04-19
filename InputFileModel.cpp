#include "InputFileModel.h"

#include <iostream>
#include <fstream>

#include <QBrush>

InputFileModel::InputFileModel(InputFile* pFile, QObject* parent)
    : QAbstractTableModel(parent)
    , m_pFile(pFile)
{
}

Qt::ItemFlags InputFileModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
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
    switch (role)
    {
    case Qt::DisplayRole:
        {
            if (index.column() == 0)
                return QString::number(index.row() + 1);

            return m_pFile->getCellValue(index.row(), index.column() - FRAMECOUNT_COLUMN);
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
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;

        setCachedFileData(index.row(), index.column() - FRAMECOUNT_COLUMN, value.toString());
        writeFileOnDisk();

        return true;
    }

    return false;
}

void InputFileModel::setCachedFileData(int rowIdx, int colIdx, QString val)
{
    m_pFile->setCellValue(rowIdx, colIdx, val);
    assert(m_pFile->getCellValue(rowIdx, colIdx) == val);
}

void InputFileModel::writeFileOnDisk()
{
    std::ofstream file;
    file.open(m_pFile->getPath().toStdString());

    // Iterate across the data frame-by-frame to write a new line
    const TtkFileData& data = m_pFile->getData();

    for (int i = 0; i < data.count(); i++)
    {
        std::string frameData = "";

        for (int j = 0; j < NUM_INPUT_COLUMNS; j++)
        {
            frameData += data[i][j].toStdString();
            frameData += ",";
        }

        frameData.pop_back();

        file << frameData;
        file << "\n";
    }

    file.close();
}
