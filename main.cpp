#include "rpikeyerterm.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RPiKeyerTerm w;
    w.show();
    return a.exec();
}
