#include "interpreter.h"
#include "luacontrol.h"

#include <QFileInfo>

Interpreter::Interpreter(Console* console, QWidget *parent)
    : QWidget(parent)
{
    this->console = console;
    process = new QProcess(this);
    options.clear();

    luaPath = "lua";

    connect(console, SIGNAL(emitInput(QByteArray)), this, SLOT(writeInput(QByteArray)));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(atFinish(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(started()), this, SLOT(atStart()));
}

void Interpreter::run(Source* source)
{
    if (source == NULL) return;

    if (source->doesExist()) {
        fileName = source->getFileName();
        execute();
    } else {
        if (tempFile.open()) {
            tempFile.resize(0); // truncate file
            if (tempFile.write(source->text().toAscii()) == -1) {
                // failed to create temp file
                return; // TODO add error handling
            }
            tempFile.close();
            fileName = tempFile.fileName();
            source->setTempFileName(fileName);
            execute();
        }
    }
}
void Interpreter::runFile(const QString &file)
{
    if (QFileInfo(file).exists()) {
        fileName = file;
        execute();
    }
}


// TODO add lua 5.0 and 5.2 support
void Interpreter::execute()
{
    // TODO platform specific, lua mustn't be found
    // TODO implement specifiing path to lua executable

    console->open();
    console->writeSystem(QString("Starting program ")
        .append(fileName).append('\n'));

    options << "-e" << "io.stdout:setvbuf 'no'";
    process->start(luaPath, options << "--" << fileName);
}

void Interpreter::atFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    console->writeSystem(QString("Program exited whit code ")
        .append(QString::number(exitCode)).append('\n'));
    console->close();
    options.clear();
    emit changedRunningState(false);
    emit finished();
}

void Interpreter::atStart()
{
    emit started();
    emit changedRunningState(true);
}
