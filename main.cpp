#include "rpikeyerterm.h"

#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // this allows local libraries to be found
    qDebug()<<a.libraryPaths();
    a.addLibraryPath(a.applicationDirPath());
    qDebug()<<a.libraryPaths();
    RPiKeyerTerm w;
    w.show();
    return a.exec();
}
