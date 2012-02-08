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
    void debug(Source* source);

    void runFile(const QString &file);

public slots:
    void kill() { process->kill(); }

signals:
    void changedRunningState(bool running);

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
    void writeInput(QByteArray input) { process->write(input); }

    void readStandardOutput() {
        console->writeOutput(process->readAllStandardOutput());
    }
    void readStandardError() {
        console->writeError(process->readAllStandardError());
    }

    void started() { emit changedRunningState(true); }
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // INTERPRETER__H
