#pragma once

#include <QTableView>

class InputFileModel;
class QLabel;

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(QLabel* friendLabel, QWidget* parent = nullptr);

    inline QLabel* getLabel() const {return m_pFriendLabel;}

public: // events
    void keyPressEvent(QKeyEvent* event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseReleaseEvent(QMouseEvent* event) override;

public: // connect

private:
    QLabel* m_pFriendLabel;
};

