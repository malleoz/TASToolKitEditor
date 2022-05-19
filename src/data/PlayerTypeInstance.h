#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "include/Definitions.h"

class InputFileMenu;
class InputTableView;
class InputFileHandler;

class QTableView;

enum class PlayerType
{
    Player,
    Ghost
};

class PlayerTypeInstance : public QObject
{
    Q_OBJECT
public:
    PlayerTypeInstance(const PlayerType type, QObject* parent = nullptr);

    void setupUI(QWidget* parent = nullptr);

public: // connect
    /// @return Returns only false on parse error, return true on cancel or successful load
    bool openFile();
    bool importFile();

    void closeFile();

    void reloadFile();

private:
    bool openFile(const QString& filePath);

    bool userClosedPreviousFile();

    void adjustUiOnFileLoad(const Centering centering);
    void adjustUiOnFileClose();



public: // Setters / Getters
    inline void setMenu(InputFileMenu* menu) {m_pMenu = menu;}


    inline QVBoxLayout* getLayout() {return qVLayout;}
    inline QLabel* getLabel() {return qLabel;}

    inline InputTableView* getTableView() {return m_pTableView;}
    inline InputFileMenu* getMenu() {return m_pMenu;}

    inline InputFileHandler* getFileHandler() {return m_pFileHandler;}

    inline bool isLoaded() {return m_loaded;}

signals:
    void fileClosed();

private:
    QVBoxLayout* qVLayout;
    QLabel* qLabel;

    QTableView* qRKGTable;

    InputTableView* m_pTableView;
    InputFileMenu* m_pMenu;

    InputFileHandler* m_pFileHandler;


    const PlayerType m_type;
    bool m_loaded;

private:
    static const int FRAMECOUNT_COLUMN_WIDTH = 40;
    static const int BUTTON_COLUMN_WIDTH = 20;
    static const int STICK_COLUMN_WIDTH = 25;
    static const int PAD_COLUMN_WIDTH = 35;
};
