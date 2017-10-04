#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T12:26:10
#
#-------------------------------------------------

QT       += core gui svg
CONFIG += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HeatCam
TEMPLATE = app
DESTDIR = ./bin
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
RCC_DIR = ./rcc
UI_DIR = ./ui

SOURCES += main.cpp\
        mainwindow.cpp \
    visualizationform.cpp \
    imagegraphicsviewform.cpp \
    temperaturedialog.cpp \
    analyzationform.cpp

HEADERS  += mainwindow.h \
    visualizationform.h \
    imagegraphicsviewform.h \
    temperaturedialog.h \
    analyzationform.h

FORMS    += mainwindow.ui \
    visualizationform.ui \
    imagegraphicsviewform.ui \
    temperaturedialog.ui \
    analyzationform.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/qwt/lib/release/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/qwt/lib/debug/ -lqwt
else:unix: LIBS += -L$$PWD/qwt/lib/ -lqwt

INCLUDEPATH += $$PWD/qwt
DEPENDPATH += $$PWD/qwt

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/qwt/lib/release/libqwt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/qwt/lib/debug/libqwt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/qwt/lib/release/qwt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/qwt/lib/debug/qwt.lib
else:unix: PRE_TARGETDEPS += $$PWD/qwt/lib/libqwt.a
