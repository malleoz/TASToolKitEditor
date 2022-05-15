#include "InputTableView.h"

#include "include/Definitions.h"
#include "data/InputFileModel.h"

#include <QKeyEvent>


InputTableView::InputTableView(QWidget* parent)
    : QTableView(parent)
    , startSelectionIndex()
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
//        QModelIndexList indexList = selectedIndexes();


        // If user presses Delete on row selection, delete row.
        // Otherwise, reset cell to default value.

        if (index.column() == 0)
        {
            model()->removeRow(index.row(), index);
            selectRow(index.row());
            return;
        }

//        model()->setData(index, RESET_MACRO);
    }

    QTableView::keyPressEvent(event);
}


void InputTableView::mousePressEvent(QMouseEvent* event)
{
    QTableView::mousePressEvent(event);

    startSelectionIndex = indexAt(event->pos());

    if (startSelectionIndex.column() == 0)
    {
        selectRow(startSelectionIndex.row());
        holdFrameColumn = true;
    }
}

void InputTableView::mouseMoveEvent(QMouseEvent* event)
{
    QTableView::mouseMoveEvent(event);

    if (holdFrameColumn)
    {
        QModelIndex index = indexAt(event->pos());
        if (startSelectionIndex != index)
        {
            selectAllRowsOnFrameColumn(index);
        }
    }

}

void InputTableView::mouseReleaseEvent(QMouseEvent* event)
{
    QTableView::mouseReleaseEvent(event);

    if (holdFrameColumn)
    {
        selectAllRowsOnFrameColumn(indexAt(event->pos()));
    }

    holdFrameColumn = false;
}

void InputTableView::selectAllRowsOnFrameColumn(const QModelIndex& index)
{
    if (index.column() == 0)
    {
        QItemSelectionModel* selectionModel = this->selectionModel();
        QItemSelection selection = selectionModel->selection();

        selectionModel->clearSelection();
        selectionModel->select(QItemSelection(startSelectionIndex, index), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}
