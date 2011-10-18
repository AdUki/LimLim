#ifndef LUADEBUGGER_H
#define LUADEBUGGER_H

#include <QObject>

class LuaDebugger : public QObject
{
    Q_OBJECT
public:
    explicit LuaDebugger(QObject *parent = 0);

signals:

public slots:

};

#endif // LUADEBUGGER_H
