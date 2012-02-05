#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QSet>
#include <QTabWidget>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

#include "source.h"
#include "breakpoint.h"

/** This is manager for files with source code. Manager have some opened
  * files and some of these files are modified. It can create, open, save,
  * edit and close these files.
  *
  * TODO add breakpoints - list of breakpoints, they will load after open file (like breakpoints manager)
  * TODO add graphical file manager
  */
class Editor : public QTabWidget
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent);

    Source* currentSource();
    QList<Breakpoint*> getBreakpoints() { return breakpoints; }

    void lock();
    void unlock();

public slots:
    Source* newSource();
    Source* openSource();
    Source* openSource(const QString& filename);

    void saveCurrentSource();
    void saveCurrentSourceAs();
    void saveAllSources();
    bool closeAllSources(); // call this before exit

    void debugLine(QString file, unsigned line);
    void debugClear();

private:
    Source* addSource(Source* source);

    int markerHandle;
    Source *debugSource;

    QList<Breakpoint*> breakpoints;

private slots:
    void closeTabWithSource(int i);
    void updateSourceNames();
};

#endif // EDITOR_H
