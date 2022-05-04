#pragma once

#include "InputFile.h"
#include "Definitions.h"

#include <QAbstractTableModel>


class InputFileModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    /// Class for storing latest table edits
    struct CellEditAction
    {
        CellEditAction(int row = 0, int col = 0, QString prev = "", QString cur = "");

        bool operator==(const CellEditAction& rhs);

        inline void flipValues()
        {
            QString temp = m_cur;
            m_cur = m_prev;
            m_prev = temp;
        }

        int m_rowIdx;
        int m_colIdx;

        QString m_prev;
        QString m_cur;
    };

    typedef QStack<CellEditAction> TTKStack;


public:
    InputFileModel(InputFileHandler* fileHandler, const TTKFileData data, const Centering centering, QObject* parent = nullptr);
    virtual ~InputFileModel() override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;


    inline TTKFileData& getData() {return m_fileData;}
    inline Centering getCentering() const {return m_fileCentering;}

    void swapCentering();

    void addActionToStack(CellEditAction action);
    void undo();
    void redo();

private:
    bool inputValid(const QModelIndex& index, const QVariant& value) const;

private:
    InputFileHandler* m_pFileHandler;

    TTKFileData m_fileData;
    Centering m_fileCentering;

    TTKStack undoStack;
    TTKStack redoStack;

    const QVector<int> BUTTON_COL_IDXS{ 0, 1, 2 };
};
