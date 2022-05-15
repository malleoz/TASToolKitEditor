#include "InputTableView.h"

#include "include/Definitions.h"
#include "data/InputFileModel.h"

#include <QKeyEvent>


InputTableView::InputTableView(QWidget* parent)
    : QTableView(parent)
    , holdFrameColumn(false)
{
}

void InputTableView::keyPressEvent(QKeyEvent* event)
{
    // Allow user to scroll up or down with the use of the arrow keys
    int key = event->key();
    QModelIndex index = currentIndex();

    if (key == Qt::Key_Up)
    {
        if (index.row() == 0)
            return;

        if (index.column() == 0)
        {
            selectRow(index.row() - 1);
            return;
        }
    }
    else if(key == Qt::Key_Down)
    {
        if (index.row() == model()->rowCount() - 1) {
            model()->insertRow(index.row() + 1, index);

            selectRow(index.row() + 1);
            return;
        }

        if (index.column() == 0)
        {
            selectRow(index.row() + 1);
            return;
        }
    }
    else if(key == Qt::Key_Left)
    {
        if (index.column() == 0)
        {
            selectRow(index.row());
            return;
        }
    }
    else if (key == Qt::Key_Delete)
    {
        QItemSelectionModel* selectionModel = this->selectionModel();
        QModelIndexList selectedRowList = selectionModel->selectedRows();

        for (int i = selectedRowList.count() - 1; i >= 0; i--)
        {
            QModelIndex mIndex = selectedRowList[i];
            model()->removeRow(mIndex.row(), mIndex);
        }

        InputFileModel* model = reinterpret_cast<InputFileModel*>(this->model());
        model->resetData(selectedIndexes());
    }

    QTableView::keyPressEvent(event);
}


void InputTableView::mousePressEvent(QMouseEvent* event)
{
    const QModelIndex index = indexAt(event->pos());
    if (index.column() == 0)
    {
        setSelectionBehavior(SelectionBehavior::SelectRows);
        holdFrameColumn = true;
    }

    QTableView::mousePressEvent(event);
}

void InputTableView::mouseMoveEvent(QMouseEvent* event)
{
    selectAllRowsOnFrameColumn(event);

    QTableView::mouseMoveEvent(event);
}

void InputTableView::mouseReleaseEvent(QMouseEvent* event)
{
    selectAllRowsOnFrameColumn(event);

    holdFrameColumn = false;

    QTableView::mouseReleaseEvent(event);

    setSelectionBehavior(SelectionBehavior::SelectItems);
}

void InputTableView::selectAllRowsOnFrameColumn(QMouseEvent* event)
{
    if (holdFrameColumn)
    {
        const QModelIndex index = indexAt(event->pos());
        if (index.column() == 0)
        {
            setSelectionBehavior(SelectionBehavior::SelectRows);
        }
        else
        {
            setSelectionBehavior(SelectionBehavior::SelectItems);
        }
    }
}
