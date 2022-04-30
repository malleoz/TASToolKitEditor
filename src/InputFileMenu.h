#pragma once

#include <QMenu>

class InputFileMenu : public QMenu
{
    Q_OBJECT
public:
    InputFileMenu(QString title,
                  QString shortcutPrefix,
                  QWidget* parent = Q_NULLPTR);

    inline QAction* getUndo() const { return m_pUndo; }
    inline QAction* getRedo() const { return m_pRedo; }

    inline QAction* getCenter7() const { return m_pCenter7; }
    inline void     setCenter7(bool centerOn7) { m_pCenter7->setChecked(centerOn7); }

    inline QAction* getClose() const { return m_pClose; }

private:
    QAction* m_pUndo;
    QAction* m_pRedo;

    QAction* m_pCenter7;

    QAction* m_pClose;
};
