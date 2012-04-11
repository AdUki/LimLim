#include <QtGui/QApplication>
#include <QDir>
#include <QDebug>
#include "luacontrol.h"
#include "global.h"

QString APP_DIR_PATH;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    APP_DIR_PATH = QApplication::applicationDirPath();

#ifdef Q_WS_WIN
    APP_DIR_PATH.replace(QRegExp("[^/.]+$"),"");
    APP_DIR_PATH.replace('/','\\');
#else
    APP_DIR_PATH.append(QDir::separator());
#endif

    LuaControl *mainWin = new LuaControl;

    mainWin->show();

    return app.exec();
}
