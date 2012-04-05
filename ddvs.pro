#-------------------------------------------------
#
# Project created by QtCreator 2011-11-21T15:41:46
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = ddvs
TEMPLATE = app

!win32-msvc*:QMAKE_CXXFLAGS_WARN_ON = -Wextra

SOURCES += main.cpp\
        mainwindow.cpp \
    zoomwidget.cpp \
    datatype.cpp \
    additemdialog.cpp \
    newmemberswidget.cpp \
    pointer.cpp \
    edititemdialog.cpp \
    expression.cpp \
    statement.cpp \
    function.cpp \
    interpreter.cpp

HEADERS  += mainwindow.h \
    zoomwidget.h \
    datatype.h \
    additemdialog.h \
    newmemberswidget.h \
    pointer.h \
    edititemdialog.h \
    statement.h \
    ast.h \
    expression.h \
    skipper.h \
    function.h \
    interpreter.h \
    error.h \
    annotation.h \
    types.h \
    statement_def.h \
    function_def.h \
    expression_def.h

linux:INCLUDEPATH += /usr/include/boost-1_48/
macx:INCLUDEPATH += /Users/chris/boost_1_49_0/
win32:INCLUDEPATH += C:\\dev\\include\\boost_1_49_0

FORMS += mainwindow.ui \
    zoomwidget.ui \
    additemdialog.ui \
    newmemberswidget.ui \
    edititemdialog.ui

RESOURCES += \
    icons.qrc


