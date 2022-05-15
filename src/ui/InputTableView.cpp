#include "InputTableView.h"

#include "include/Definitions.h"
#include "data/InputFileModel.h"

#include <QKeyEvent>


InputTableView::InputTableView(QWidget* parent)
    : QTableView(parent)
    , contextMenu("Context Menu", this)
    , holdFrameColumn(false)
{
    copyAction = new QAction("Copy");
    pasteAction = new QAction("Paste");
    deleteAction = new QAction("Delete");

    copyAction->setShortcut(QString("Ctrl+C"));
    pasteAction->setShortcut(QString("Ctrl+V"));
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    contextMenu.addAction(copyAction);
    contextMenu.addAction(pasteAction);

    contextMenu.addSeparator();

    contextMenu.addAction(deleteAction);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested, this, [&](const QPoint& pos){contextMenu.exec(mapToGlobal(pos));});
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

        // lambda because QModelIndex doesn't override operator>()
        // so std::greater doesn't work
        std::sort(selectedRowList.begin(), selectedRowList.end(), [](QModelIndex a, QModelIndex b) {return b < a;});

        for (QModelIndex mIndex : selectedRowList)
        {
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
