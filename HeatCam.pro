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
    heatmapdialog.cpp \
        mainwindow.cpp \
    visualizationform.cpp \
    imagegraphicsviewform.cpp \
    temperaturedialog.cpp \
    analyzationform.cpp

HEADERS  += mainwindow.h \
    heatmapdialog.h \
    visualizationform.h \
    imagegraphicsviewform.h \
    temperaturedialog.h \
    analyzationform.h

FORMS    += mainwindow.ui \
    heatmapdialog.ui \
    visualizationform.ui \
    imagegraphicsviewform.ui \
    temperaturedialog.ui \
    analyzationform.ui




unix:!macx: LIBS += -L$$PWD/../../qwt-6.1/lib/ -lqwt

INCLUDEPATH += $$PWD/../../qwt-6.1
DEPENDPATH += $$PWD/../../qwt-6.1
