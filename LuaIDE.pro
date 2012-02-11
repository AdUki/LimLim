# -------------------------------------------------
# Project created by QtCreator 2011-10-03T23:07:41
# -------------------------------------------------
QT += core \
    gui
TARGET = LuaIDE
TEMPLATE = app
SOURCES += src/main.cpp \
    src/interpreter.cpp \
    src/luacontrol.cpp \
    src/debugger.cpp \
    src/console.cpp \
    src/source.cpp \
    src/editor.cpp \
    src/variablewatcher.cpp \
    src/breakpoint.cpp \
    src/variable.cpp \
    src/variablemodel.cpp
HEADERS += src/interpreter.h \
    src/luacontrol.h \
    src/debugger.h \
    src/console.h \
    src/source.h \
    src/editor.h \
    src/variablewatcher.h \
    src/breakpoint.h \
    src/variable.h \
    src/variablemodel.h
LIBS += -lqscintilla2
RESOURCES += icons.qrc
OTHER_FILES += controller.lua
OBJECTS_DIR = build/
MOC_DIR = build/
RCC_DIR = build/
