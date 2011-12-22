#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QSet>
#include <QTabWidget>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

#include "source.h"

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

public slots:
    const Source* newSource();
    const Source* openSource();
    const Source* openSource(const QString& filename);

    void saveCurrentSource();
    void saveCurrentSourceAs();
    void saveAllSources();
    bool closeAllSources(); // call this before exit

private:
    const Source* addSource(Source* source);
    Source* currentSource();

private slots:
    void closeTabWithSource(int i);
    void updateSourceNames();
};

#endif // EDITOR_H
