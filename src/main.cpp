#include <QtWidgets/QApplication>
#include <QDir>
#include <QDebug>
#include "luacontrol.h"

QString APP_DIR_PATH;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LuaControl *mainWin = new LuaControl;

    mainWin->show();

    return app.exec();
}
