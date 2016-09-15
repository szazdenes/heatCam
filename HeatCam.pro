#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T12:26:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HeatCam
TEMPLATE = app
DESTDIR = ./bin
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
RCC_DIR = ./rcc
UI_DIR = ./ui


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
