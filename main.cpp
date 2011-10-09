#include <QtGui/QApplication>
#include "luacontrol.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	LuaControl *mainWin = new LuaControl;
	mainWin->show();

	return app.exec();
}
