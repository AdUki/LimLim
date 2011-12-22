#include "debugger.h"

/*
	Lua debugger class

	This debugger can be used remotely, whitout lua interpreter,
	to debbug embedded lua applications.

*/
LuaDebugger::LuaDebugger(QObject *parent) :
    QObject(parent)
{
}
