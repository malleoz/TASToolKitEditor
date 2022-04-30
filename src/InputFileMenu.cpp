#include "InputFileMenu.h"

InputFileMenu::InputFileMenu(QAction* undo,
                             QAction* redo,
                             QAction* center7,
                             QAction* close,
                             QString* title,
                             QWidget* parent)
    : QMenu(parent),
      m_pUndo(undo),
      m_pRedo(redo),
      m_pCenter7(center7),
      m_pClose(close)
{
// TODO: init
}
