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

    enum DebugStatus { Running, Waiting, On, Off };

signals:
    void waitingForCommand(bool flag);

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
    QProcess *remdebug;

    DebugStatus status;
    QByteArray output;
    QByteArray input;

    bool autoRun;

private slots:
    void controlParser();
    void controlFinish(int exitCode, QProcess::ExitStatus exitStatus);

    void controlWrite(QByteArray input) { remdebug->write(input); }
    
    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);
};

#endif // DEBUGGER_H
