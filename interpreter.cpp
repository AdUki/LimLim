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
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
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

void Interpreter::debug(Source* source)
{
    options << "-e" << "require 'remdebug.engine'.start()";
    this->run(source);
}

// TODO add lua 5.0 and 5.2 support
void Interpreter::execute()
{
    // TODO platform specific, lua mustn't be found
    // TODO implement specifiing path to lua executable

    console->open();
    process->terminate();
    options << "-e" << "io.stdout:setvbuf 'no'";
    process->start(luaPath, options << "--" << fileName);
}

void Interpreter::kill()
{
    process->kill();
    onClose();
}

void Interpreter::onClose()
{
    console->close();
    options.clear();
}

void Interpreter::writeInput(QByteArray input)
{
    process->write(input);

}

void Interpreter::readStandardOutput()
{
    console->writeOutput(process->readAllStandardOutput());
}

void Interpreter::readStandardError()
{
    console->writeError(process->readAllStandardError());
}

void Interpreter::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    onClose();
}

/*
int Interpreter::run(QString code)
{
	clear();
	setReadOnly(false);
	process = new QProcess(this);
	// TODO WARINING next line is platform specific
	// TODO change next line so that lua will execute code from file path
	process->start("lua", QStringList() << "-e" << "io.stdout:setvbuf 'no'" << "-e" << code << "--");
	connect(process, SIGNAL(readyReadStandardError()), this, SLOT(writeError()));
	connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(writeOutput()));
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(end(int, QProcess::ExitStatus)));

	//installEventFilter(new UserInputFilter(this));

        // TODO change colors of text according to System color setup
        setTextColor(QColor::fromRgb(255,255,255,255));

	return 0;
}*/
