#include "InputTableViewNew.h"

#include <QKeyEvent>

#include "InputFileModel.h"


InputTableViewNew::InputTableViewNew(InputFileModel* model,QWidget* parent)
    : QTableView(parent)
{
    setModel(model);
}

void InputTableViewNew::keyPressEvent(QKeyEvent* event)
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
        if (index.row() >= model()->rowCount() - 1) {
            model()->insertRows(index.row() + 1, 1);

//            InputFileModel* pModel = reinterpret_cast<InputFileModel*>(model());
//            pModel->setTemplateRow(index.row());
//            pModel->insertRows(index.row() + 1, 1);
        }

        selectRow(index.row() + 1);
        return;
    }

    QTableView::keyPressEvent(event);
}
