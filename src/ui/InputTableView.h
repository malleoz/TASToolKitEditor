#pragma once

#include <QTableView>
#include <QMenu>

class InputTableView : public QTableView
{
    Q_OBJECT
public:
    InputTableView(QWidget* parent = nullptr);

protected: // events
    void keyPressEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

protected:
    void copySelection();
    void pasteSelection();

    void deleteSelection();

private:
    int abstractSelection(QModelIndexList& o_selection);

    void selectAllRowsOnFrameColumn(QMouseEvent* event);

private: // context menu
    QMenu contextMenu;

    QAction* copyAction;
    QAction* pasteAction;
    QAction* deleteAction;

private:
    bool m_holdFrameColumn;
};

