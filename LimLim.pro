######################################################################
# Automatically generated by qmake (2.01a) ut 17. IV 10:39:58 2012
######################################################################

CONFIG+=DEBUG
TEMPLATE = app
<<<<<<< HEAD
TARGET = 
DEPENDPATH += . build forms src
INCLUDEPATH += .

# Input
HEADERS += src/breakpoint.h \
           src/console.h \
           src/debugger.h \
           src/editor.h \
           src/global.h \
           src/hideeventwatcher.h \
           src/interpreter.h \
           src/luacontrol.h \
           src/source.h \
           src/stack.h \
           src/watcher.h
=======
SOURCES += src/main.cpp \
    src/interpreter.cpp \
    src/luacontrol.cpp \
    src/debugger.cpp \
    src/console.cpp \
    src/source.cpp \
    src/editor.cpp \
    src/breakpoint.cpp \
    src/watcher.cpp \
    src/stack.cpp
HEADERS += src/interpreter.h \
    src/luacontrol.h \
    src/debugger.h \
    src/console.h \
    src/source.h \
    src/editor.h \
    src/breakpoint.h \
    src/watcher.h \
    src/stack.h
LIBS += -lqscintilla2
RESOURCES += icons.qrc
OTHER_FILES += limdebug/controller.lua \
    limdebug/engine.lua
OBJECTS_DIR = build/
MOC_DIR = build/
RCC_DIR = build/
>>>>>>> 1d30653513c0087875d32720ba9454fdb0c8b1c8
FORMS += forms/interpreter.ui
SOURCES += src/breakpoint.cpp \
           src/console.cpp \
           src/debugger.cpp \
           src/editor.cpp \
           src/hideeventwatcher.cpp \
           src/interpreter.cpp \
           src/luacontrol.cpp \
           src/main.cpp \
           src/source.cpp \
           src/stack.cpp \
           src/watcher.cpp
RESOURCES += icons.qrc
