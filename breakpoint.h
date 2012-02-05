#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QObject>

class Breakpoint : public QObject
{
Q_OBJECT
public:
    Breakpoint(QString *file, int line, QObject *parent);

    QString* file;
    int line;
};


#endif // BREAKPOINT_H
