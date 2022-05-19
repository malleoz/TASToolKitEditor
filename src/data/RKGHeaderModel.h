#pragma ocne

#include "include/RKGDefinitions.h"

#include <QAbstractTableModel>

class RKGHeaderModel : public QAbstractTableModel
{
public:
    RKGHeaderModel(QObject *parent = nullptr);

public: // inherit
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public: // setter / getter
    void setHeader(const RKGHeader& header) {m_header = header;}
    const RKGHeader& getHeader() const {return m_header;}

private:
    RKGHeader m_header;

private:
    static const int EDITABLE_HEADER_COUNT = 15;
};

