#include "ui/TTKMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TTKMainWindow w;
    w.setAttribute(Qt::WA_DeleteOnClose);
    w.show();
    return a.exec();
}
