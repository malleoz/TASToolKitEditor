#pragma once

#include <QTableView>

class InputFileModel;

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;
};

