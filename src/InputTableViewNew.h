#pragma once

#include <QTableView>

class InputFileModel;

class InputTableViewNew : public QTableView
{
public:
    InputTableViewNew(InputFileModel* model, QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* event) override;

private:
//    InputFileModel* m_pInputFileModel;
};
