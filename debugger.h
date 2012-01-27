#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>
#include "console.h"

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(QObject *parent = 0);

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
    QProcess *remdebug;
    Console *console;
    DebugStatus status;

private slots:
    void controlParser();
    void controlFinish(int exitCode, QProcess::ExitStatus exitStatus);

    void controlWrite(QByteArray input) { remdebug->write(input); }
};

#endif // DEBUGGER_H
