#include "luainterpret.h"
#include "lua.hpp"
#include "luacontrol.h"

#include <stdio.h>
#include <stdlib.h>

LuaInterpret::LuaInterpret(QWidget *parent) :
	QPlainTextEdit(parent)
{
	setReadOnly(true);
	setOverwriteMode(false);
	// connect all 'observers' breakpoint()
	// ...

}

int LuaInterpret::run(QString code)
{
	clear();
	// ##################################################################
	// TODO WARINING!!!! fix this part !!! this is platform specific !!!!
	// ##################################################################
	process = new QProcess(this);
	process->start("lua5.1", QStringList() << "-e" << code);
	connect(process, SIGNAL(readyReadStandardError()), this, SLOT(writeError()));
	connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(writeOutput()));
	connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(end(int, QProcess::ExitStatus)));

	return 0;
}

int LuaInterpret::debug(QString code)
{
	return 0;
}


void LuaInterpret::writeError()
{
	appendPlainText(process->readAllStandardError());
}

void LuaInterpret::writeOutput()
{
	appendPlainText(process->readAllStandardOutput());
}

void LuaInterpret::end(int value, QProcess::ExitStatus)
{
	appendPlainText(QString("Program exited with %1\n").arg(value));
	setReadOnly(true);
	emit finished(true);
}

void LuaInterpret::stop()
{
	process->kill();
}

void LuaInterpret::keyPressEvent ( QKeyEvent * event )
{
	if (!isReadOnly()) {
		// ######################################################################
		// TODO WARINING!!!! fix condition !!! this may be platform specific !!!!
		// ###  new line character has not got ascii value bat strange value  ###
		// ######################################################################
		if (event->text().compare(QString('\n')) == 3) {
			sendInput(inputBuff);
			inputBuff.clear();
		} else {
			inputBuff.append(event->text().toAscii());
			QPlainTextEdit::keyPressEvent(event);
		}
	}
}

void LuaInterpret::sendInput(const QByteArray& input)
{
	process->write(input.data(), input.size());
	char newLine = '\n';
	process->write(&newLine, 1);
}
