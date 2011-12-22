#include "breakpoint.h"

Breakpoint::Breakpoint(QFile* source, unsigned line, QObject *parent) :
        QObject(parent)
{
    this->source = source;
    this->line = line;
}
