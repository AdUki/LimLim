#include <QtGui/QApplication>
#include "luacontrol.h"

// TODO add argument processing
//      [file] [...]
//      file contains source code that should be loaded
//      ... are arguments passed to input of lua code
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	LuaControl *mainWin = new LuaControl;

	mainWin->show();

	return app.exec();
}
