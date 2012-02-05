#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

#include "editor.h"
#include "console.h"
#include "breakpoint.h"

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(Editor *editor, QObject *parent = 0);

    void setConsole(Console *console);

    enum DebugStatus { Running, Off };

public slots:
    void start();
    void stop();

    void stepOver();
    void stepIn();
    void run();

    DebugStatus getStatus() { return status; }

private:
    Console *console;
    Editor *editor;
    QProcess *remdebug;

    DebugStatus status;
    QByteArray output;

private slots:
    void controlParser();
    void controlFinish(int exitCode, QProcess::ExitStatus exitStatus);

    void controlWrite(QByteArray input) { remdebug->write(input); }
};

#endif // DEBUGGER_H
