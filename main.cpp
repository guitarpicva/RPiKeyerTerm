#include "rpikeyerterm.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // this allows local libraries to be found
    a.addLibraryPath(a.applicationDirPath());
    RPiKeyerTerm w;
    w.show();
    return a.exec();
}
