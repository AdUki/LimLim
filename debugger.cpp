#include "debugger.h"

static const QByteArray StartMessage = QByteArray("Start program you want to debug\n");
static const QByteArray EndMessage = QByteArray("Program finished\n");

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

    autoRun = true;

    connect(remdebug, SIGNAL(readyRead()), this, SLOT(controlParser()));
    connect(remdebug, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(controlFinish(int, QProcess::ExitStatus)));

    connect(editor, SIGNAL(breakpointSet(int,QString)), this, SLOT(breakpointSet(int,QString)));
    connect(editor, SIGNAL(breakpointDeleted(int,QString)), this, SLOT(breakpointDeleted(int,QString)));
}

void Debugger::setConsole(Console *console)
{
    this->console = console;
    connect(console, SIGNAL(emitInput(QByteArray)), this, SLOT(controlWrite(QByteArray)));
}

void Debugger::start()
{
    if (console != NULL) console->open();

    // start RemDebug controller
    remdebug->start("lua", QStringList() << "-e" << "io.stdout:setvbuf 'no'" << "--" << "controller.lua");


}

void Debugger::stop()
{
    remdebug->kill();
}

void Debugger::giveCommand(QByteArray command)
{
    status = Running;
    emit waitingForCommand(false);
    remdebug->write(command);
    if (console != NULL) console->writeError(command);

}

void Debugger::controlParser()
{
    output.append(remdebug->readAll());

    // Return if RemDebug didn't write whole status
    if (!output.endsWith("> ") && !output.endsWith(EndMessage) && !output.endsWith(StartMessage))
        return;

    if (console != NULL) console->writeOutput(output);

    if (output.startsWith("Paused:")) {
        QRegExp rx("^Paused:( line ){0,1}(\\d*)( watch ){0,1}(\\d*) file (.*)\n.*");

        rx.indexIn(output);

        int line = rx.cap(2).toInt();   // get line number
        int watch = rx.cap(4).toInt();  // get watch id
        QString file = rx.cap(5);

        editor->debugLine(file, line);

    } else if (output.startsWith(EndMessage)) {

    } else if (output.startsWith(StartMessage)) {
        status = On;
    }

    if (output.endsWith("> ")) {

        // Controller initialization
        if (status == On) {
            status = Waiting;

            // Lock editor for editing
            editor->lock();

            // Read breakpoints
            QList<Breakpoint*> breakpoints = editor->getBreakpoints();
            QList<Breakpoint*>::iterator iter = breakpoints.begin();

            for (; iter != breakpoints.end(); iter++) {
                Breakpoint *br = static_cast<Breakpoint*> (*iter);
                QString command = QString("setb %1 %2\n").arg(br->file).arg(br->line);
                input.append(command.toAscii());
            }

            if (autoRun) input.append("run\n");
        }

        // If there is input, write as command to console.
        if (input.isEmpty()) {
            status = Waiting;
            emit waitingForCommand(true);
        } else {
            static QBuffer buffer(&input);

            status = Running;
            if (!buffer.isOpen()) buffer.open(QIODevice::ReadOnly);

            QByteArray command(buffer.readLine());
            if (console != NULL) console->writeError(command);
            remdebug->write(command);

            if (buffer.atEnd()) {
                buffer.close();
                input.clear();
            }
        }
    }

    output.clear();
}

void Debugger::controlFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    status = Off;
    editor->debugClear();
    editor->unlock();

    input.clear();
    if (console != NULL) {
        console->writeOutput(QString("Process exit code %1\nProcess exit status %2\n")
                .arg(exitCode)
                .arg(exitStatus).toAscii());
        console->close();
    }
}

void Debugger::breakpointSet(int line, QString file)
{
    QByteArray command(QString("setb %1 %2\n").arg(file).arg(line).toAscii());

    switch (status) {
    case Waiting:
        remdebug->write(command);
        if (console != NULL) console->writeError(command);
        break;
    case Running: input.append(command);
    default: ;
    }
}

void Debugger::breakpointDeleted(int line, QString file)
{
    QByteArray command(QString("delb %1 %2\n").arg(file).arg(line).toAscii());

    switch (status) {
    case Waiting:
        remdebug->write(command);
        if (console != NULL) console->writeError(command);
        break;
    case Running: input.append(command);
    default: ;
    }
}
