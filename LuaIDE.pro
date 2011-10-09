#-------------------------------------------------
#
# Project created by QtCreator 2011-10-03T23:07:41
#
#-------------------------------------------------

QT       += core gui

TARGET = LuaIDE
TEMPLATE = app


SOURCES += main.cpp\
    luaeditor.cpp \
    luainterpret.cpp \
    luacontrol.cpp

HEADERS  += \
    luaeditor.h \
    luainterpret.h \
    luacontrol.h

FORMS    +=

LIBS	+= -lqscintilla2 -llua5.1

RESOURCES += \
    icons.qrc
