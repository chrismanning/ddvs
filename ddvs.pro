#-------------------------------------------------
#
# Project created by QtCreator 2011-11-21T15:41:46
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = ddvs
TEMPLATE = app

!win32-msvc*:QMAKE_CXXFLAGS= -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    additemdialog.cpp \
    newmemberswidget.cpp \
    edititemdialog.cpp \
    expression.cpp \
    statement.cpp \
    function.cpp \
    interpreter.cpp \
    graphicsitems.cpp

HEADERS  += mainwindow.h \
    additemdialog.h \
    newmemberswidget.h \
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
    expression_def.h \
    graphicsview.h \
    graphicsitems.h

linux:INCLUDEPATH += /usr/include/boost-1_49/
macx:INCLUDEPATH += /Users/chris/boost_1_49_0/
win32:INCLUDEPATH += C:\\dev\\include\\boost_1_49_0

#DEFINES+= QT_NO_DEBUG_OUTPUT

FORMS += mainwindow.ui \
    additemdialog.ui \
    newmemberswidget.ui \
    edititemdialog.ui

RESOURCES += \
    icons.qrc

OTHER_FILES +=


