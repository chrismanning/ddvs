#-------------------------------------------------
#
# Project created by QtCreator 2011-11-21T15:41:46
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = ddvs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    zoomwidget.cpp \
    datatype.cpp \
    additemdialog.cpp \
    newmemberswidget.cpp

HEADERS  += mainwindow.h \
    zoomwidget.h \
    datatype.h \
    additemdialog.h \
    newmemberswidget.h

FORMS    += mainwindow.ui \
    zoomwidget.ui \
    additemdialog.ui \
    newmemberswidget.ui

RESOURCES += \
    icons.qrc

QMAKE_CXXFLAGS += -std=c++0x
