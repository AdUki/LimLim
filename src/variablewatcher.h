#ifndef VARIABLEWATCHER_H
#define VARIABLEWATCHER_H

#include <QTableWidget>
#include "debugger.h"

class VariableWatcher : public QTableWidget
{
Q_OBJECT
public:
    explicit VariableWatcher(const Debugger *debugger);

private slots:
    void newWatch();
    void deleteWatch();
};

#endif // VARIABLEWATCHER_H
