#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QWidget>
#include <QProcess>
#include <QTemporaryFile>


#include "editor.h"
#include "source.h"
#include "console.h"

class Interpreter : public QWidget
{
Q_OBJECT
public:
    explicit Interpreter(Console* console, QWidget *parent = 0);

    void run(Source* source);

    /** Function adds debug mode
      * when you want to debug code run this ALWAYS before run()
      */
    void addDebug() { options << "-e" << "require 'remdebug.engine'.start()"; }

    void runFile(const QString &file);

public slots:
    void kill() { process->kill(); }

signals:
    void changedRunningState(bool running);
    void finished();
    void started();

private:
    QString fileName;
    QString luaPath;

    QProcess *process;
    QStringList options;

    QString compiler;
    QString interpreter;

    Console* console;

    QTemporaryFile tempFile;

    void execute();

    void ignoreEnvironmentVars() { options << "-E"; }
    void setInteractiveMode() { options << "-i"; }

private slots:
    void writeInput(const QByteArray& input) { process->write(input); }

    void readStandardOutput() {
        console->writeOutput(process->readAllStandardOutput());
    }
    void readStandardError() {
        console->writeError(process->readAllStandardError());
    }
    void atFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void atStart();
};

#endif // INTERPRETER__H
