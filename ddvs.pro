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
    newmemberswidget.cpp \
    pointer.cpp \
    edititemdialog.cpp

HEADERS  += mainwindow.h \
    zoomwidget.h \
    datatype.h \
    additemdialog.h \
    newmemberswidget.h \
    pointer.h \
    edititemdialog.h

unix:INCLUDEPATH += /usr/include/boost-1_47/

FORMS    += mainwindow.ui \
    zoomwidget.ui \
    additemdialog.ui \
    newmemberswidget.ui \
    edititemdialog.ui

RESOURCES += \
    icons.qrc


