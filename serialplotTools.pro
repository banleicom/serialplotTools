#-------------------------------------------------
#
# Project created by QtCreator 2019-01-05T09:57:56
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

LIBS += -lpthread -lwsock32 -lws2_32
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialplotTools
TEMPLATE = app

#qwt setting
QWT_ROOT = D:\Qt5.10.1\Qwt-6.1.3 #qwt控件安装位置，注意与qwtconfig.pri配置文件配合修改
include ( $${QWT_ROOT}/features/qwt.prf )
#end qwt setting

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
        main.cpp \
        mainwindow.cpp \
    serialthread.cpp

HEADERS += \
        mainwindow.h \
    serialthread.h

FORMS += \
        mainwindow.ui

RC_FILE  = wave.rc
