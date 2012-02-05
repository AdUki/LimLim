#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QObject>

class Breakpoint
    : public QObject
{

    Q_OBJECT

public:
    explicit Breakpoint(const QString &file, int line, QObject *parent = 0);

    QString file;
    int line;
};


#endif // BREAKPOINT_H
