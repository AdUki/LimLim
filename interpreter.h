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

public slots:
    void run(Source* source);
    void debug(Source* source);
    void kill();

signals:
    void executionChanged(bool running);

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
    void onClose();

    void ignoreEnvironmentVars() { options << "-E"; }
    void setInteractiveMode() { options << "-i"; }

private slots:
    void writeInput(QByteArray input);

    void readStandardOutput();
    void readStandardError();

    void finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // INTERPRETER__H
