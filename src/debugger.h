#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

#include "editor.h"
#include "console.h"
#include "breakpoint.h"
#include "interpreter.h"

static const QByteArray RunCommand = QByteArray("run\n");
static const QByteArray StepOverCommand = QByteArray("over\n");
static const QByteArray StepIntoCommand = QByteArray("step\n");

static const QByteArray ExecuteCommand = QByteArray("exec ");
static const QByteArray EvaluateCommand = QByteArray("eval ");

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(Editor *editor, Console *console, QObject *parent = 0);

    enum DebugStatus { Running, Waiting, On, Off };

signals:
    void waitingForCommand(bool flag);

    void started();
    void finished();

public slots:
    void start();
    void stop();

    void stepOver() { giveCommand(StepOverCommand); }
    void stepIn() { giveCommand(StepIntoCommand); }
    void run() { giveCommand(RunCommand); }

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
    void stateChange(bool running);

    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);
};

#endif // DEBUGGER_H
