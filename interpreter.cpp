#include "interpreter.h"
#include "lua5.1/lua.hpp"
#include "luacontrol.h"

#include <stdio.h>
#include <stdlib.h>
#include <QKeyEvent>
#include <QTimer>

/*
	This class need to acces LuaIntrpret atributes and it just
	filters input.

	TODO Try to implement eventFilter inside LuaInterpret class
		 or try to add more functionality from keyPressEvent
		 from LuaInterpret here.
*//*
class UserInputFilter : public QObject {

public:
	UserInputFilter( QObject *parent = 0 ) : QObject( parent ) {
		this->parent = static_cast<LuaInterpret*>(parent);
	}

private:
	LuaInterpret *parent;

protected:
	bool eventFilter( QObject *dist, QEvent *event )
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent *mousePress = static_cast<QMouseEvent*>(event);
			if (mousePress->button() == Qt::LeftButton)
			{

				parent->setFocus();
				return true; // block left mouse button
			}
		}
		//else if (event->type() == QEvent::KeyPress)

		return false;
	}
};*/

/*
	Main class for interpreting lua code.

	You can also launch lua code with debug via this class.
	TODO !!!!! disable left mouse button !!!!!
*/
Interpreter::Interpreter(QWidget *parent) :
        QTextEdit(parent)
{
	setUndoRedoEnabled(false);
	setReadOnly(true);
	setOverwriteMode(false);
	fixedPosition = 0;
	//setAttribute(Qt::WA_TransparentForMouseEvents);

	// connect all 'observers' breakpoint()
	// ...

}

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
}

int Interpreter::debug(QString code)
{
	code = "";

	return 0;
}


void Interpreter::writeError()
{
	// TODO set char format to print colorful text (setCurrentCharFormat)
        setTextColor(QColor::fromRgb(255,100,100,255));
        append(process->readAllStandardError());
}

void Interpreter::writeOutput()
{
	// TODO set char format to print colorful text (setCurrentCharFormat)
        setTextColor(QColor::fromRgb(100,100,255,255));
        append(process->readAllStandardOutput());
        setTextColor(QColor::fromRgb(0,0,0,255));
}

void Interpreter::end(int value, QProcess::ExitStatus)
{
        setTextColor(QColor::fromRgb(0,0,0,255));
	append(QString("Program exited with %1\n").arg(value));
	setReadOnly(false);
	emit finished(true);
}

void Interpreter::stop()
{
//        process->kill();
}

void Interpreter::keyPressEvent ( QKeyEvent * event )
{
    QTextEdit::keyPressEvent(event);
    return;
	if (!isReadOnly()) {
		if (event->key() == Qt::Key_Return ||
				event->key() == Qt::Key_Enter) {
			sendInput(inputBuff);
			inputBuff.clear();
		// TODO next line is ugly solution... refactor
		// TODO add this filtering to InputFilter
		} else if (event->key() >= Qt::Key_Space &&
				   event->key() <= Qt::Key_ydiaeresis) {
			inputBuff.append(event->text().toAscii());
			QTextEdit::keyPressEvent(event);
		} else if (event->key() == Qt::Key_Backspace) {
			inputBuff.remove(inputBuff.size()-1, 1);
			QTextEdit::keyPressEvent(event);
		}
	}
}

void Interpreter::sendInput(const QByteArray& input)
{
	process->write(input.data(), input.size());
	process->write(QByteArray("\n"));
}


