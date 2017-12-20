#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T22:27:02
#
#-------------------------------------------------
QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
QT       += core gui
QT       += network
QT       += gamepad
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GS_template_Windows
TEMPLATE = app


SOURCES += \
    WifiWrapper/payload.cpp \
    WifiWrapper/satellitelink.cpp \
    main.cpp \
    mainwindow.cpp \
    Qcustomplot/qcustomplot.cpp \
    Graphsource/graphsource.cpp \
    manualcontrol.cpp

HEADERS  += \
    WifiWrapper/basics.h \
    WifiWrapper/payload.h \
    WifiWrapper/satellitelink.h \
    mainwindow.h \
    Qcustomplot/qcustomplot.h \
    manualcontrol.h \
    XInputWrapper/iwindows_xinput_wrapper.h
INCLUDEPATH += \
           WifiWrapper \
           Qcustomplot \
           Graphsource

FORMS    += \
    mainwindow.ui \
    manualcontrol.ui

LIBS += -lXInput
