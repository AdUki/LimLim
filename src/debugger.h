#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

#include "editor.h"
#include "console.h"
#include "breakpoint.h"
#include "interpreter.h"

class QTreeWidgetItem;

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
    void localsChanged(QList<QTreeWidgetItem*> *locals);
    void globalsChanged(QList<QTreeWidgetItem*> *globals);
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

    void setWatch(QTreeWidgetItem *watch);

    void setAutoRun(bool enabled) { autoRun = enabled; }
    void setUpdateLocals(bool enabled) { updateLocals = enabled; }
    void setUpdateGlobals(bool enabled) { updateGlobals = enabled; }
    void setUpdateStack(bool enabled) { updateStack = enabled; }

private:
    Console *console;
    Editor *editor;
    Interpreter *remdebug;

    DebugStatus status;
    QByteArray output;
    QByteArray input;

    QList<QTreeWidgetItem*> watches;
    QList<QTreeWidgetItem*> tables;
    QSet<QString> expandedTables;

    bool autoRun;
    bool updateLocals;
    bool updateGlobals;
    bool updateStack;

    inline void giveCommand(const QByteArray& command);
    QString getWatchName(const QTreeWidgetItem *watch);

private slots:
    void parseInput(const QByteArray& remdebugOutput);
    void stateChange(bool running);

    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);
};

#endif // DEBUGGER_H
