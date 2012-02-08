#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

#include "editor.h"
#include "console.h"
#include "breakpoint.h"
#include "interpreter.h"

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(Editor *editor, Console *console, QObject *parent = 0);

    enum DebugStatus { Running, Waiting, On, Off };

signals:
    void waitingForCommand(bool flag);
    void changedRunningState(bool running);
    void started();
    void finished();

public slots:
    void start();
    void stop();

    void stepOver() { giveCommand(QByteArray("over\n")); }
    void stepIn() { giveCommand(QByteArray("step\n")); }
    void run() { giveCommand(QByteArray("run\n")); }

    void setAutoRun(bool enabled) { autoRun = enabled; }

    DebugStatus getStatus() { return status; }
    void giveCommand(QByteArray command);

private:
    Console *console;
    Editor *editor;
    Interpreter *remdebug;

    DebugStatus status;
    QByteArray output;
    QByteArray input;

    bool autoRun;

private slots:
    void parseInput(QByteArray remdebugOutput);
    void atFinish();

    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);
};

#endif // DEBUGGER_H
