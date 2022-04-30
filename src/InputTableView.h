#pragma once

#include <QTableView>

class InputFileModel;

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(InputFileModel* model, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;

    //Depricated
    InputTableView(QWidget* parent = nullptr);
};

