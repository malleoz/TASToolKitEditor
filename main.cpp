#include "TASToolKitEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TASToolKitEditor w;
    w.show();
    return a.exec();
}
