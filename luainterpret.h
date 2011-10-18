#ifndef LUA_INTERPRET_H
#define LUA_INTERPRET_H

#include <QTextEdit>
#include <QProcess>

class LuaInterpret : public QTextEdit
{
    Q_OBJECT

public:
	explicit LuaInterpret(QWidget *parent = 0);

	int run(QString code);
	int debug(QString code);

protected:
	virtual void keyPressEvent ( QKeyEvent * event );

signals:
	void breakpoint(); // OK
	void finished(bool status);

public slots:
	void stop();

private:
	QProcess *process;
	QByteArray inputBuff;
	int fixedPosition;

	void sendInput(const QByteArray& input);

private slots:
	void writeError();
	void writeOutput();
	void end(int value, QProcess::ExitStatus);
};

#endif // LUA_INTERPRET_H
