#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <QObject>
#include <QProcess>

class Debugger : public QObject
{
Q_OBJECT
public:
    explicit Debugger(QObject *parent = 0);

public slots:
    void start();
    void stop();

    void stepOver();
    void stepIn();
    void run();

private:
    QProcess remdebug;

};

#endif // DEBUGGER_H
