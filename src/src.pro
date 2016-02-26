#-------------------------------------------------
#
# Project created by QtCreator 2016-02-23T11:25:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = src
TEMPLATE = app
CONFIG += c++14
INCLUDEPATH += $$PWD/../lib/glm

SOURCES += main.cpp\
        mainwindow.cpp \
    simulator.cpp \
    configs.cpp \
    pathgraph.cpp \
	simframe.cpp \
    pathgraphviewer.cpp

HEADERS  += mainwindow.h \
    simulator.h \
    configs.h \
    pathgraph.h \
	simframe.h \
    pathgraphviewer.h

FORMS    += mainwindow.ui \
    pathgraphviewer.ui
