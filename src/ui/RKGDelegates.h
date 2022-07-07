#pragma once

#include <QStyledItemDelegate>

class RKGComboBoxDelegate : public QStyledItemDelegate
{
public:
    RKGComboBoxDelegate(const QStringList& cBoxList, QWidget* parent = nullptr);

public: // override
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
    const QStringList m_cBoxList;
};

