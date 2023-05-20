QT       += core gui network multimedia

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
    rbndialog.cpp \
    rpikeyerterm.cpp

HEADERS += \
    FIRFilterLib_global.h \
    alphabet.h \
    firbpfilter.h \
    firfilterlib.h \
    firfilterwidget.h \
    logdialog.h \
    logutils.h \
    macrodialog.h \
    maidenhead.h \
    rbndialog.h \
    rpikeyerterm.h

FORMS += \
    logdialog.ui \
    macrodialog.ui \
    rbndialog.ui \
    rpikeyerterm.ui

RESOURCES += \
    qresource.qrc

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/aarch64-linux-gnu/ -lgpiod

INCLUDEPATH += $$PWD/../../../../usr/include
DEPENDPATH += $$PWD/../../../../usr/include

unix:!macx: LIBS += -L$$PWD/../RPiKeyerTerm -lFIRFilterLib

#INCLUDEPATH += $$PWD/../
#DEPENDPATH += $$PWD/../

message(TARGET: $$TARGET)
message(PWD value: $$PWD)
message(OUT_PWD: $$OUT_PWD)
DESTFOLDER = $$(HOME)/RPiKeyerTerm
message(Destination base folder: $$DESTFOLDER)

installation.path = $$DESTFOLDER
installation.files = $$OUT_PWD/RPiKeyerTerm $$PWD/RPiKeyerTerm.sh $$PWD/libFIRFilterLib.so.1.0.0

# once all files are in place adjust their permissions and create symlinks
postinstall.path = $$DESTFOLDER
unix:postinstall.extra = chmod +x $$DESTFOLDER/RPiKeyerTerm.sh;ln -s $$DESTFOLDER/libFIRFilterLib.so.1.0.0 $$DESTFOLDER/libFIRFilterLib.so.1.0;ln -s $$DESTFOLDER/libFIRFilterLib.so.1.0.0 $$DESTFOLDER/libFIRFilterLib.so.1;ln -s $$DESTFOLDER/libFIRFilterLib.so.1.0.0 $$DESTFOLDER/libFIRFilterLib.so

INSTALLS += installation postinstall
