#pragma once

#include "InputFile.h"
#include "Definitions.h"

#include <QAbstractTableModel>
#include <QUndoStack>

class InputFileModel;

class CellEditCommand : public QUndoCommand
{
public:
    CellEditCommand(InputFileModel* pModel, const QModelIndex& index, const QString oldVal, const QString newVal);
    void undo() override;
    void redo() override;

private:
    InputFileModel* m_pModel;
    QModelIndex m_index;
    QString m_oldVal;
    QString m_newVal;
};

class InputFileModel : public QAbstractTableModel
{
    Q_OBJECT

        friend class CellEditCommand;

public:
    InputFileModel(const TTKFileData data, const Centering centering, QObject* parent = nullptr);
    virtual ~InputFileModel() override;

public: // inherit
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

public:
    inline TTKFileData& getData() {return m_fileData;}
    inline Centering getCentering() const {return m_fileCentering;}
    inline QUndoStack* getUndoStack() { return &m_undoStack; }

    void swapCentering();

    void swap(InputFileModel* rhs);

signals:
    void fileToBeWritten(const TTKFileData& data);

private:
    bool inputValid(const QModelIndex& index, const QVariant& value) const;

protected:
    void emitDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

    TTKFileData m_fileData;

private:
    Centering m_fileCentering;

    QUndoStack m_undoStack;

    const QVector<int> BUTTON_COL_IDXS{ 0, 1, 2 };
};
