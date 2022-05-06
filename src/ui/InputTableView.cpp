#include "InputTableView.h"

#include "include/Definitions.h"
#include "data/InputFileModel.h"

#include <QKeyEvent>


InputTableView::InputTableView(QWidget* parent)
    : QTableView(parent)
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

        selectRow(index.row() - 1);
        return;
    }
    else if (key == Qt::Key_Down)
    {
        if (index.row() == model()->rowCount() - 1) {
            model()->insertRow(index.row() + 1, index);
        }

        selectRow(index.row() + 1);
        return;
    }
    else if (key == Qt::Key_Delete)
    {
        // If user presses Delete on row selection, delete row.
        // Otherwise, reset cell to default value.

        if (index.column() == 0)
        {
            model()->removeRow(index.row(), index);
            selectRow(index.row());
            return;
        }

        model()->setData(index, RESET_MACRO);
    }

    QTableView::keyPressEvent(event);
}

/*
void InputTableView::mouseMoveEvent(QMouseEvent* event)
{
}

void InputTableView::mousePressEvent(QMouseEvent* event)
{
}

void InputTableView::mouseReleaseEvent(QMouseEvent* event)
{
}
*/
