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
    -llua
RESOURCES += icons.qrc
OTHER_FILES += images/process-stop.png \
    images/run.png \
    images/process-stop.png \
    images/lua.png \
    images/run.png \
    images/process-stop.png \
    images/lua.png \
    images/compile.png \
    images/debug/step-over.png \
    images/debug/step-out.png \
    images/debug/step-into-instruction.png \
    images/debug/step-into.png \
    images/debug/step-instruction.png \
    images/debug/run-cursor.png \
    images/debug/run.png \
    images/debug/execute-to-cursor.png \
    images/debug/execute-from-cursor.png
