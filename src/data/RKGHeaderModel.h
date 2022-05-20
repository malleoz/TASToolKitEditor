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

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;


    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

public: // setter / getter
    void setHeader(const RKGHeader& header) {m_header = header;}
    const RKGHeader& getHeader() const {return m_header;}

private:
    QString dataFromHeader(const QModelIndex& index) const;
    bool dataToHeader(const QModelIndex& index, const QString& data);

private:
    RKGHeader m_header;
    int m_currentLapCount;

private:
    static const int EDITABLE_HEADER_COUNT = 15;
    static const QStringList EDITABLE_HEADER_NAMES;

    static const int LAPTIME_HEADER_INDEX = 9;
};
