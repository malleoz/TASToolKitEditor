#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "Definitions.h"

class InputFileMenu;
class InputTableView;
class InputFileHandler;


enum class PlayerType
{
    Player,
    Ghost
};

class PlayerTypeInstance : public QObject
{
public:
    PlayerTypeInstance(const PlayerType type, QObject* parent = nullptr);

    void setupUI(QWidget* parent = nullptr);

public: // connect
    void openFile(QWidget* main);
    void closeFile();
    void saveFile(const TTKFileData& data);
    void reloadFile();

private:
    bool userClosedPreviousFile(QWidget* main);

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

private:
    QVBoxLayout* qVLayout;
    QLabel* qLabel;

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
