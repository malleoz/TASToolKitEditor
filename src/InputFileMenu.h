#pragma once

#include <QMenu>

class InputFileMenu : public QMenu
{
    Q_OBJECT
public:
    InputFileMenu(QString title,
                  QString shortcutPrefix,
                  QWidget* parent = Q_NULLPTR);

    QAction* getUndo() const { return m_pUndo; };
    QAction* getRedo() const { return m_pRedo; };

    QAction* getCenter7() const { return m_pCenter7; };
    void     setCenter7(bool centerOn7) { m_pCenter7->setChecked(centerOn7); };

    QAction* getClose() const { return m_pClose; };

private:
    QAction* m_pUndo;
    QAction* m_pRedo;

    QAction* m_pCenter7;

    QAction* m_pClose;
};
