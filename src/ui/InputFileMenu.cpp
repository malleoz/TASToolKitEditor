#include "InputFileMenu.h"

InputFileMenu::InputFileMenu(QString title,
                             QString shortcutPrefix,
                             QWidget* parent)
    : QMenu(parent)
{
    // Child button creation
    m_pClose = new QAction(this);
    m_pUndo = new QAction(this);
    m_pRedo = new QAction(this);
    m_pCenter7 = new QAction(this);

    // Naming
    setTitle(title);
    m_pClose->setText("Close File");
    m_pUndo->setText("Undo");
    m_pRedo->setText("Redo");
    m_pCenter7->setText("7 Centered");

    // Set shortcuts
#if QT_CONFIG(shortcut)
    m_pClose->setShortcut(shortcutPrefix + QString("Esc"));
    m_pUndo->setShortcut(shortcutPrefix + QString("Z"));
    m_pRedo->setShortcut(shortcutPrefix + QString("Y"));
#endif

    // Create hierarchy
    addAction(m_pClose);
    addAction(m_pUndo);
    addAction(m_pRedo);
    addAction(m_pCenter7);

    // Hide player/ghost menu by default
    menuAction()->setVisible(false);

    // Initially disable undo/redo items
    m_pUndo->setEnabled(false);
    m_pRedo->setEnabled(false);

    // Mark the centering button as checkable
    m_pCenter7->setCheckable(true);
}
