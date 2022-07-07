#include "InputTableView.h"

#include "include/Definitions.h"
#include "data/InputFileModel.h"

#include <QKeyEvent>
#include <QMessageBox>


InputTableView::InputTableView(QWidget* parent)
    : QTableView(parent)
    , contextMenu("Context Menu", this)
    , m_holdFrameColumn(false)
{
    // set up actions
    copyAction = new QAction("Copy");
    pasteAction = new QAction("Paste");
    deleteAction = new QAction("Delete");

    copyAction->setShortcut(QString("Ctrl+C"));
    pasteAction->setShortcut(QString("Ctrl+V"));
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));

    // this is needed to enable shortcuts working on the table itself
    this->addAction(copyAction);
    this->addAction(pasteAction);
    this->addAction(deleteAction);


    // fill context menu
    contextMenu.addAction(copyAction);
    contextMenu.addAction(pasteAction);

    contextMenu.addSeparator();

    contextMenu.addAction(deleteAction);

    // initiate context menu and connects
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested, this, [&](const QPoint& pos){contextMenu.exec(mapToGlobal(pos));});

    connect(copyAction, &QAction::triggered, this, &InputTableView::copySelection);
    connect(pasteAction, &QAction::triggered, this, &InputTableView::pasteSelection);
    connect(deleteAction, &QAction::triggered, this, &InputTableView::deleteSelection);
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

    QTableView::keyPressEvent(event);
}


void InputTableView::copySelection()
{
    QModelIndexList processingList;
    int columnCount = abstractSelection(processingList);

    if (columnCount > 0)
    {
        InputFileModel* model = dynamic_cast<InputFileModel*>(this->model());
        model->copyIndices(processingList, columnCount);
    }
}

void InputTableView::pasteSelection()
{
    QModelIndexList processingList;
    int columnCount = abstractSelection(processingList);

    if (columnCount > 0)
    {
        InputFileModel* model = dynamic_cast<InputFileModel*>(this->model());

        if (!model->pasteIndices(processingList, columnCount))
        {
            QMessageBox::warning(this, "Paste Error", "The pasted data does not match the selected cells.");
        }
    }
}

void InputTableView::deleteSelection()
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

    InputFileModel* model = dynamic_cast<InputFileModel*>(this->model());
    model->resetData(selectedIndexes());
}


int InputTableView::abstractSelection(QModelIndexList& o_selection)
{
    QItemSelectionModel* selectionModel = this->selectionModel();

    if (selectionModel->selectedIndexes().count() == 0)
        return -1;

    QModelIndexList selectedRowList = selectionModel->selectedRows();

    int columnCount = 0;

    if (selectedRowList.count() == 0)
    {
        QModelIndexList selectedItems = selectedIndexes();
        std::sort(selectedItems.begin(), selectedItems.end());

        o_selection.append(QItemSelection(selectedItems.first(), selectedItems.last()).indexes());
        columnCount = selectedItems.last().column() - selectedItems.first().column() + 1;
    }
    else
    {
        QModelIndexList altRowList = selectionModel->selectedRows(FRAMECOUNT_COLUMN + NUM_INPUT_COLUMNS - 1);
        std::sort(selectedRowList.begin(), selectedRowList.end());
        std::sort(altRowList.begin(), altRowList.end());

        if (selectedRowList.count() != altRowList.count())
            return -1;

        for (int i = 0; i < selectedRowList.count(); i++)
        {
            o_selection.append(QItemSelection(selectedRowList[i], altRowList[i]).indexes());
        }

        columnCount = FRAMECOUNT_COLUMN + NUM_INPUT_COLUMNS;
    }

    return columnCount;
}


void InputTableView::mousePressEvent(QMouseEvent* event)
{
    const QModelIndex index = indexAt(event->pos());
    if (index.column() == 0)
    {
        setSelectionBehavior(SelectionBehavior::SelectRows);
        m_holdFrameColumn = true;
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

    m_holdFrameColumn = false;

    QTableView::mouseReleaseEvent(event);

    setSelectionBehavior(SelectionBehavior::SelectItems);
}

void InputTableView::selectAllRowsOnFrameColumn(QMouseEvent* event)
{
    if (m_holdFrameColumn)
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
