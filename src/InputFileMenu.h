#pragma once

#include <QMenu>

class InputFileMenu : public QMenu
{
    Q_OBJECT
public:
    InputFileMenu(QAction* undo,
                  QAction* redo,
                  QAction* center7,
                  QAction* close,
                  QString* title,
                  QWidget* parent = Q_NULLPTR);

private:
    QAction* m_pUndo;
    QAction* m_pRedo;

    QAction* m_pCenter7;

    QAction* m_pClose;
};
