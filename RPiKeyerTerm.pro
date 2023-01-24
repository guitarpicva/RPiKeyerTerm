QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    logdialog.cpp \
    logutils.cpp \
    macrodialog.cpp \
    maidenhead.cpp \
    main.cpp \
    rpikeyerterm.cpp \
    scrollingdigitlabel.cpp

HEADERS += \
    alphabet.h \
    logdialog.h \
    logutils.h \
    macrodialog.h \
    maidenhead.h \
    rpikeyerterm.h \
    scrollingdigitlabel.h

FORMS += \
    logdialog.ui \
    macrodialog.ui \
    rpikeyerterm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qresource.qrc

DISTFILES += \
    README.md \
    RPiKeyerTerm.jpg

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/aarch64-linux-gnu/ -lgpiod

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include
