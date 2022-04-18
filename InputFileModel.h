#pragma once

#include "InputFile.h"

#include <QAbstractTableModel>

class InputFileModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    InputFileModel(InputFile* pFile, QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
    InputFile* m_pFile;
};