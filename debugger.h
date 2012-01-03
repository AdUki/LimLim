#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QFile>

class LuaDebugger : public QObject
{
    Q_OBJECT
public:
    explicit LuaDebugger(QObject *parent = 0);

signals:

public slots:

};

#endif // DEBUGGER_H
