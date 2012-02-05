#include "debugger.h"

/*
	Lua debugger class

	This debugger can be used remotely, whitout lua interpreter,
	to debbug embedded lua applications.

*/
Debugger::Debugger(Editor *editor, QObject *parent) :
    QObject(parent)
{
    remdebug = new QProcess(this);
    this->editor = editor;

    connect(remdebug, SIGNAL(readyRead()), this, SLOT(controlParser()));
    connect(remdebug, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(controlFinish(int, QProcess::ExitStatus)));
}

void Debugger::setConsole(Console *console)
{
    this->console = console;
    connect(console, SIGNAL(emitInput(QByteArray)), this, SLOT(controlWrite(QByteArray)));
}

void Debugger::start()
{
    status = Running;
    if (console != NULL) console->open();
    // TODO platform specific, controller.lua mustn't be found

    // start RemDebug controller
    remdebug->start("lua5.1", QStringList() << "-e" << "io.stdout:setvbuf 'no'" << "--" << "controller.lua");

    // set breakpoints from editor

}

void Debugger::stop()
{
    remdebug->terminate();
}

// TODO change strings with commands to macros or functions or something

void Debugger::stepOver()
{
    remdebug->write("over\n");
    if (console != NULL) console->writeError("over\n");
}

void Debugger::stepIn()
{
    remdebug->write("step\n");
    if (console != NULL) console->writeError("step\n");
}

void Debugger::run()
{
    remdebug->write("run\n");
    if (console != NULL) console->writeError("run\n");
}

void Debugger::controlParser()
{
    output.append(remdebug->readAll());

    if (!output.endsWith("> ") && !output.endsWith("Program finished\n") && !output.endsWith("Program started\n"))
        return;

    if (console != NULL) console->writeOutput(output);

    if (output.startsWith("Paused:")) {
        QRegExp rx("^Paused:( line ){0,1}(\\d*)( watch ){0,1}(\\d*) file (.*)\n.*");

        rx.indexIn(output);

        int line = rx.cap(2).toInt();
        int watch = rx.cap(4).toInt();
        QString file = rx.cap(5);

        editor->debugLine(file, line);

    } else if (output.startsWith("Program finished")) {

    }

    output.clear();
}

void Debugger::controlFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    status = Off;
    editor->debugClear();

    if (console != NULL) console->close();
}
