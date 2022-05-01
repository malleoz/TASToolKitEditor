#pragma once

#include <QTableView>

class InputFileModel;
class QLabel;

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(QLabel* friendLabel, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;

    inline QLabel* getLabel() const {return m_pFriendLabel;}

public: // connect

private:
    QLabel* m_pFriendLabel;
};

