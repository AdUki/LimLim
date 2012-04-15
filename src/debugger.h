#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

#include "editor.h"
#include "console.h"
#include "breakpoint.h"
#include "interpreter.h"

class QTreeWidgetItem;

static const QByteArray RunCommand = QByteArray("run\n");
static const QByteArray StepOverCommand = QByteArray("over\n");
static const QByteArray StepIntoCommand = QByteArray("step\n");
static const QByteArray LocalCommand = QByteArray("local\n");
static const QByteArray ExecuteCommand = QByteArray("exec ");
static const QByteArray EvaluateCommand = QByteArray("eval ");
static const QByteArray TableCommand = QByteArray("table ");

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(Editor *editor, Console *console, QObject *parent = 0);
    ~Debugger();

    enum DebugStatus { Running, Waiting, On, Off };

    DebugStatus getStatus() { return status; }
    void setLocalsUpdating(bool eval);

signals:
    void waitingForCommand(bool status);
    void luaStateChanged();
    // TODO maybe change to "QList* getLocals()" but then watcher needs reference to debugger
    void localsChanged(QList<QTreeWidgetItem*> *locals);
    void stackChanged(QStringList *stack);

    void started();
    void finished();
    void updateActions(bool isOn);

public slots:
    void start();
    void stop();

    void stepOver();
    void stepIn();
    void run();

    void updateWatch(QTreeWidgetItem *watch);
    void updateWatches(QList<QTreeWidgetItem*> *watches);
    void updateTable(QTreeWidgetItem *table);

    void setAutoRun(bool enabled) { autoRun = enabled; }
    void setUpdateLocals(bool enabled) { updateLocals = enabled; }

private:
    Console *console;
    Editor *editor;
    Interpreter *remdebug;

    DebugStatus status;
    QByteArray output;
    QByteArray input;

    QList<QTreeWidgetItem*> watches;
    QList<QTreeWidgetItem*> tables;

    bool autoRun;
    bool updateLocals;

    inline void giveCommand(const QByteArray& command);

private slots:
    void parseInput(const QByteArray& remdebugOutput);
    void stateChange(bool running);

    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);
};

#endif // DEBUGGER_H
