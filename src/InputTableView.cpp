#include "InputTableView.h"

#include <QKeyEvent>


InputTableView::InputTableView(QWidget* parent) :
    QTableView(parent)
{
}

void InputTableView::keyPressEvent(QKeyEvent* event)
{
    // Allow user to scroll up or down with the use of the arrow keys
    int key = event->key();
    QModelIndex index = currentIndex();

//    if (key == Qt::Key_Up)
//    {
//        if (index.row() == 0)
//            return;

//        selectRow(index.row() - 1);
//        return;
//    }
//    else if (key == Qt::Key_Down)
//    {
//        if (index.row() >= model()->rowCount() - 1) {
//            InputFileModel* pModel = (InputFileModel*)model();
//            pModel->setTemplateRow(index.row());
//            pModel->insertRows(index.row() + 1, 1);
//        }

//        selectRow(index.row() + 1);
//        return;
//    }

    QTableView::keyPressEvent(event);
}
