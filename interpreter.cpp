#include "interpreter.h"
#include "luacontrol.h"

Interpreter::Interpreter(Editor* editor, Console* console, QWidget *parent)
    : QWidget(parent)
{
    this->console = console;
    this->editor = editor;
    process = new QProcess(this);
    options.clear();

    connect(console, SIGNAL(emitInput(QByteArray)), this, SLOT(writeInput(QByteArray)));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readStandardError()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
}

void Interpreter::run()
{
    Source* current = editor->currentSource();
    if (current != NULL)
        execute(current);
}

void Interpreter::debug()
{

}

void Interpreter::execute(Source* source)
{
    console->open();

    if (source->doesExist()) process->start("lua5.1", options << "-e" << "io.stdout:setvbuf 'no'" << "--" << source->getFileName());
    else {
        if (tempFile.open()) {
            tempFile.resize(0); // truncate file
            if (tempFile.write(source->text().toAscii()) == -1) {
                return; // TODO add error handling
            }
            tempFile.close();
            process->start("lua", options << "-e" << "io.stdout:setvbuf 'no'" << "--" << tempFile.fileName());
        } // TODO add error handling
    }
}

void Interpreter::kill()
{
    process->kill();
    terminate();
}

void Interpreter::terminate()
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
    terminate();
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
