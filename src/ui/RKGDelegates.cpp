#include "RKGDelegates.h"

#include <QComboBox>

RKGComboBoxDelegate::RKGComboBoxDelegate(const QStringList& cBoxList, QWidget* parent)
    : QStyledItemDelegate(parent)
    , m_cBoxList(cBoxList)
{
}

QWidget* RKGComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* rkgCB = new QComboBox(parent);
    rkgCB->addItems(m_cBoxList);

    rkgCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    return rkgCB;
}

void RKGComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QComboBox* rkgCB = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(rkgCB);

    const QString text = index.data().toString();
    const int cbIndex = rkgCB->findText(text);

    if (cbIndex >= 0)
        rkgCB->setCurrentIndex(cbIndex);
}

void RKGComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* rkgCB = qobject_cast<QComboBox*>(editor);
    Q_ASSERT(rkgCB);

    model->setData(index, rkgCB->currentIndex(), Qt::EditRole);
}
