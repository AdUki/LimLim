#include "breakpoint.h"

Breakpoint::Breakpoint(QString *file, int line, QObject *parent)
    : QObject(parent)
{
    this->file = file;
    this->line = line;
}
