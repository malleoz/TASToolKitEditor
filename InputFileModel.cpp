#include "InputFileModel.h"

#include <iostream>
#include <fstream>

#include <QAction>
#include <QBrush>
#include <QTableView>

InputFileModel::InputFileModel(InputFile* pFile, QObject* parent)
    : QAbstractTableModel(parent)
    , m_pFile(pFile)
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
            if (index.column() < 4)
                return QVariant();

            return m_pFile->getCellValue(index.row(), index.column() - FRAMECOUNT_COLUMN);
        }
    case Qt::CheckStateRole:
        {
            if (index.column() == 0 || index.column() > 3)
                return QVariant();

            QString value = m_pFile->getCellValue(index.row(), index.column() - FRAMECOUNT_COLUMN);
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

    QString prevValue = m_pFile->getCellValue(index.row(), index.column() - FRAMECOUNT_COLUMN);
    QString curValue = "";

    if (role == Qt::EditRole) {
        if (!(m_pFile->inputValid(index, value)))
            return false;

        curValue = value.toString();
    }
    else if (role == Qt::CheckStateRole)
    {
        if ((Qt::CheckState)value.toInt() == Qt::Checked)
            curValue = "1";
        else
            curValue = "0";
    }

    setCachedFileData(index.row(), index.column() - FRAMECOUNT_COLUMN, curValue);
    addToStack(CellEditAction(index.row(), index.column() - FRAMECOUNT_COLUMN, prevValue, curValue));
    writeFileOnDisk(m_pFile);

    m_pFile->getTableView()->viewport()->update();

    return false;
}

void InputFileModel::updateActionMenus()
{
    m_pFile->getMenus().undo->setEnabled(m_pFile->getUndoStack()->count() > 0);
    m_pFile->getMenus().redo->setEnabled(m_pFile->getRedoStack()->count() > 0);
}

void InputFileModel::addToStack(CellEditAction action)
{
    if (m_pFile->getRedoStack()->count() > 0)
        addToStackWithNonEmptyRedo(action);
    else
        m_pFile->getUndoStack()->append(action);
    
    updateActionMenus();
}

void InputFileModel::addToStackWithNonEmptyRedo(CellEditAction action)
{
    CellEditAction redoTop = m_pFile->getRedoStack()->top();
    redoTop.flipValues();

    // Scenario 1: user performs same action as in top of redo stack
    if (action == redoTop)
        m_pFile->getRedoStack()->pop();
    // Scenario 2: user performs action not in redo stack, so clear it first
    else
        m_pFile->getRedoStack()->clear();

    m_pFile->getUndoStack()->push(action);
}

void InputFileModel::setCachedFileData(int rowIdx, int colIdx, QString val)
{
    m_pFile->setCellValue(rowIdx, colIdx, val);
}

void InputFileModel::writeFileOnDisk(InputFile* pInputFile)
{
    std::ofstream file;
    file.open(pInputFile->getPath().toStdString());

    // Iterate across the data frame-by-frame to write a new line
    const TtkFileData& data = pInputFile->getData();

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
