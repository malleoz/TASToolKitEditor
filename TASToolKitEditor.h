#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <QtWidgets/QMainWindow>

class TASToolKitEditor : public QMainWindow
{
    Q_OBJECT

public:
    TASToolKitEditor(QWidget *parent = Q_NULLPTR);

private:
    QAction* actionUndoPlayer;
    QAction* actionRedoPlayer;
    QAction* actionUndoGhost;
    QAction* actionRedoGhost;
    QAction* actionOpenPlayer;
    QAction* actionOpenGhost;
    QAction* actionClosePlayer;
    QAction* actionCloseGhost;
    QAction* action0Centered;
    QAction* action7Centered;
    QAction* actionSwapFiles;
    QAction* actionScrollTogether;
    QWidget* centralWidget;
    QWidget* horizontalLayoutWidget;
    QHBoxLayout* mainHorizLayout;
    QVBoxLayout* playerVLayout;
    QLabel* playerLabel;
    QTableView* playerTableView;
    QVBoxLayout* ghostVLayout;
    QLabel* ghostLabel;
    QTableView* ghostTableView;
    QMenuBar* menuBar;
    QMenu* menuFile;
    QMenu* menuInputCentering;
    QMenu* menuPlayer;
    QMenu* menuGhost;
    QToolBar* mainToolBar;
    QStatusBar* statusBar;

    void setupUi();
    void setTitles();
    void setTitleNames();
    void setTitleShortcuts();
    void connectActions();

    void retranslateUi(QMainWindow* TASToolKitEditorClass)
    {
        TASToolKitEditorClass->setWindowTitle(QCoreApplication::translate("TASToolKitEditorClass", "TASToolKitEditor", nullptr));
        actionUndoPlayer->setText(QCoreApplication::translate("TASToolKitEditorClass", "Undo (Ctrl + Z)", nullptr));
#if QT_CONFIG(shortcut)
        actionUndoPlayer->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRedoPlayer->setText(QCoreApplication::translate("TASToolKitEditorClass", "Redo (Ctrl + Y)", nullptr));
#if QT_CONFIG(shortcut)
        actionRedoPlayer->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionUndoGhost->setText(QCoreApplication::translate("TASToolKitEditorClass", "Undo (Ctrl + Shift + Z)", nullptr));
#if QT_CONFIG(shortcut)
        actionUndoGhost->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+Shift+Z", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRedoGhost->setText(QCoreApplication::translate("TASToolKitEditorClass", "Redo (Ctrl + Shift + Y)", nullptr));
#if QT_CONFIG(shortcut)
        actionRedoGhost->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+Shift+Y", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOpenPlayer->setText(QCoreApplication::translate("TASToolKitEditorClass", "Open Player (Ctrl + O)", nullptr));
#if QT_CONFIG(shortcut)
        actionOpenPlayer->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOpenGhost->setText(QCoreApplication::translate("TASToolKitEditorClass", "Open Ghost (Ctrl + Shift + O)", nullptr));
#if QT_CONFIG(shortcut)
        actionOpenGhost->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Ctrl+Shift+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClosePlayer->setText(QCoreApplication::translate("TASToolKitEditorClass", "Close Player (Esc)", nullptr));
#if QT_CONFIG(shortcut)
        actionClosePlayer->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Esc", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCloseGhost->setText(QCoreApplication::translate("TASToolKitEditorClass", "Close Ghost (Shift + Esc)", nullptr));
#if QT_CONFIG(shortcut)
        actionCloseGhost->setShortcut(QCoreApplication::translate("TASToolKitEditorClass", "Shift+Esc", nullptr));
#endif // QT_CONFIG(shortcut)
        action0Centered->setText(QCoreApplication::translate("TASToolKitEditorClass", "0 Centered", nullptr));
        action7Centered->setText(QCoreApplication::translate("TASToolKitEditorClass", "7 Centered", nullptr));
        actionSwapFiles->setText(QCoreApplication::translate("TASToolKitEditorClass", "Swap Player and Ghost", nullptr));
        actionScrollTogether->setText(QCoreApplication::translate("TASToolKitEditorClass", "Scroll Together", nullptr));
        playerLabel->setText(QCoreApplication::translate("TASToolKitEditorClass", "Player", nullptr));
        ghostLabel->setText(QCoreApplication::translate("TASToolKitEditorClass", "Ghost", nullptr));
        menuFile->setTitle(QCoreApplication::translate("TASToolKitEditorClass", "File", nullptr));
        menuInputCentering->setTitle(QCoreApplication::translate("TASToolKitEditorClass", "Input Centering", nullptr));
        menuPlayer->setTitle(QCoreApplication::translate("TASToolKitEditorClass", "Player", nullptr));
        menuGhost->setTitle(QCoreApplication::translate("TASToolKitEditorClass", "Ghost", nullptr));
    } // retranslateUi
};
