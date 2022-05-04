#pragma once

#include <QTableView>

class InputFileModel;
class QLabel;

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(QWidget* parent = nullptr);

public: // events
    void keyPressEvent(QKeyEvent* event) override;
    //void mouseMoveEvent(QMouseEvent* event) override;
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseReleaseEvent(QMouseEvent* event) override;

};

