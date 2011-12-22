#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QObject>
#include <QFile>

class Breakpoint : public QObject
{
Q_OBJECT
public:
    explicit Breakpoint(QFile* source, unsigned line, QObject *parent = 0);

    void setLine(unsigned line) {this->line = line; }

    QString getFileName() { return source->fileName(); }
    unsigned getLine() { return line; }

private:
    QFile* source;
    unsigned line;
};

#endif // BREAKPOINT_H
