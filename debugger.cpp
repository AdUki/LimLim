#include "debugger.h"

/*
	Lua debugger class

	This debugger can be used remotely, whitout lua interpreter,
	to debbug embedded lua applications.

*/
Debugger::Debugger(QObject *parent) :
    QObject(parent)
{
    remdebug = new QProcess(this);

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
    remdebug->start("lua5.1", QStringList() << "-e" << "io.stdout:setvbuf 'no'" << "--" << "controller.lua");
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
    QByteArray output = remdebug->readAll();

    if (console != NULL) console->writeOutput(output);
}

void Debugger::controlFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    status = Off;

    if (console != NULL) console->close();
}
