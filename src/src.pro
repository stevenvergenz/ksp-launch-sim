#-------------------------------------------------
#
# Project created by QtCreator 2016-02-23T11:25:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simulator.cpp \
    configs.cpp \
    vec2.cpp

HEADERS  += mainwindow.h \
    vec2.h \
    simulator.h \
    configs.h

FORMS    += mainwindow.ui
