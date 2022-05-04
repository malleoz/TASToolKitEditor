#ifndef PLAYERTYPEINSTANCE_H
#define PLAYERTYPEINSTANCE_H


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
    Q_OBJECT
public:
    PlayerTypeInstance(const PlayerType type, QObject* parent = nullptr);

    void setupUI(QWidget* parent = nullptr);

public: // connect
    void openFile(QWidget* main);
    void closeFile();

    void toggleCentering();

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
};

#endif // PLAYERTYPEINSTANCE_H
