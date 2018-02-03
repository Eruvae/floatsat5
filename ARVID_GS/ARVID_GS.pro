#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T22:27:02
#
#-------------------------------------------------
QT       += core gui network serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
QT       += core gui
QT       += network
QT       += gamepad
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ARVID_GS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    WifiWrapper/payload.cpp \
    WifiWrapper/satellitelink.cpp \
    main.cpp \
    mainwindow.cpp \
    Qcustomplot/qcustomplot.cpp \
    Graphsource/graphsource.cpp \
    tcwindow.cpp \
    powerdata.cpp \
    QMeter/qmeter.cpp \
    QMeter/qmeterdrawfunctions.cpp \
    manualcontrol.cpp \
    Graphsource/tracking_plot.cpp

HEADERS  += \
    WifiWrapper/basics.h \
    WifiWrapper/payload.h \
    WifiWrapper/satellitelink.h \
    mainwindow.h \
    Qcustomplot/qcustomplot.h \
    tcwindow.h \
    powerdata.h \
    QMeter/qmeter.h \
    manualcontrol.h
INCLUDEPATH += \
           WifiWrapper \
           Qcustomplot \
           Graphsource\
           QMeter

FORMS    += \
    mainwindow.ui \
    tcwindow.ui \
    powerdata.ui \
    manualcontrol.ui


RESOURCES += \
    arvid_res.qrc
