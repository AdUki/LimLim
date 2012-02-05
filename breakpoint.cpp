#include "breakpoint.h"

Breakpoint::Breakpoint(const QString &file, int line, QObject *parent)
    : QObject(parent)
{
    this->file = file;
    this->line = line;
}
