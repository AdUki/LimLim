# -------------------------------------------------
# Project created by QtCreator 2011-10-03T23:07:41
# -------------------------------------------------
QT += core \
    gui
TARGET = LuaIDE
TEMPLATE = app
SOURCES += main.cpp \
    interpreter.cpp \
    luacontrol.cpp \
    debugger.cpp \
    console.cpp \
    breakpoint.cpp \
    source.cpp \
    editor.cpp

HEADERS += interpreter.h \
    luacontrol.h \
    debugger.h \
    console.h \
    breakpoint.h \
    source.h \
    editor.h

FORMS += 
LIBS += -lqscintilla2 \
    -llua5.1
RESOURCES += icons.qrc
