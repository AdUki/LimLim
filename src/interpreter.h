#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QDialog>
#include <QTemporaryFile>
#include <QProcess>

namespace Ui {
    class InterpreterForm;
}
class Console;
class Editor;
class Source;

// TODO implement argument managment to lua programs
class Interpreter : public QDialog
{
Q_OBJECT
public:
    explicit Interpreter(Console* console, QWidget *parent = 0);
    virtual ~Interpreter();

    void run(Source* source);

    /** Function adds debug mode
      * when you want to debug code run this ALWAYS before run()
      */
    void addDebug() { options << "-e" << "require 'remdebug.engine'.start()"; }

    void runFile(const QString &file);

    void addArgs(const QStringList& args);
    void addArg(const QString& arg);
    void clearArgs();

    void addOptions(const QStringList& options);
    void addOption(const QString& option);
    void clearOptions();

public slots:
    void kill();

signals:
    void changedRunningState(bool running);
    void finished();
    void started();

private:
    Ui::InterpreterForm *ui;

    QString fileName;
    QString luaPath;

    QProcess *process;
    QStringList options;

    QString compiler;
    QString interpreter;

    Console* console;
    QStringList args;

    QTemporaryFile tempFile;

    void execute();

    //void ignoreEnvironmentVars() { options << "-E"; }
    //void setInteractiveMode() { options << "-i"; }

private slots:
    void writeInput(const QByteArray& input);
    void readStandardOutput();
    void readStandardError();
    void atFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void atStart();
};

#endif // INTERPRETER__H
