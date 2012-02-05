#include "editor.h"

#include <QGridLayout>
#include <QFileDialog>

Editor::Editor(QWidget *parent) :
    QTabWidget(parent)
{
    markerHandle = -1;

    setTabsClosable(true);

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabWithSource(int)));
}

Source* Editor::newSource()
{
    return addSource(new Source(NULL, this));
}

Source* Editor::openSource()
{
    QString filename = QFileDialog::getOpenFileName(this,
                tr("Open Lua file"), ".",
                tr("Lua files (*.lua)\n"
                   "All Files (*)"));

    return openSource(filename);
}

Source* Editor::openSource(const QString &filename)
{
    if (filename.isEmpty()) return NULL;
    return addSource(new Source(filename, this));
}

Source* Editor::addSource(Source* source)
{
    // Search for source in tabs
    int i, tabIndex = -1;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        if (*src == *source) {
            tabIndex = i;
            break;
        }
    }
    if (tabIndex >= 0) {
        // Source already opened
        setCurrentIndex(tabIndex);
        setStatusTip(tr("Source already opened"));
    } else {
        // Try to open source
        QString sourceName = source->getName();
        if (sourceName.compare("") != 0) {
            int i = addTab(source, sourceName);
            setCurrentIndex(i);

            // connect signals and slots
            connect(source, SIGNAL(modificationChanged(bool)), this, SLOT(updateSourceNames()));
            connect(source, SIGNAL(breakpointDeleted(int,QString)), this, SIGNAL(breakpointDeleted(int,QString)));
            connect(source, SIGNAL(breakpointSet(int,QString)), this, SIGNAL(breakpointSet(int,QString)));

            // return newly opened source
            return source;
        }
        return NULL;
    }
    return source;
}

void Editor::saveCurrentSource()
{
    if (currentSource()) currentSource()->save();
    updateSourceNames();
}

void Editor::saveCurrentSourceAs()
{
    if (currentSource()) currentSource()->saveAs();
    updateSourceNames();
}

void Editor::saveAllSources()
{
    int i;
    for (i = 0; i < this->count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        src->save();
    }
}

bool Editor::closeAllSources()
{
    // TODO make simple dialog of loosing unsaved changes
    //      after closing files from a list

    Source* src;

    int i;
    for (i = 0; i < count(); i++) {
        src = static_cast<Source*>(widget(i));
        if (!src->canClose()) return false;
    }

    for (i = 0; i < count(); i++) {
         src = static_cast<Source*>(widget(i));
         delete src;
         removeTab(i);
    }

    return true;
}

void Editor::closeTabWithSource(int i)
{
    Source* src = static_cast<Source*>(this->widget(i));
    if (src->canClose()) {
        delete src;
        removeTab(i);
    }
    if (count() == 0) src = NULL;
}

void Editor::updateSourceNames()
{
    int i;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        setTabText(i, src->getName());
    }
}

Source* Editor::currentSource()
{
    if (count() > 0)
        return static_cast<Source*>(currentWidget());
    else return NULL;
}

void Editor::debugLine(QString file, unsigned line)
{
    debugClear();

    debugSource = NULL;

    int i;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        if (QFileInfo(src->getFileName()) == QFileInfo(file)) {
            setCurrentIndex(i);
            debugSource = src; // Found file in tabs
            break;
        }
    }
    if (debugSource == NULL) { // Try to open referenced file
        debugSource = openSource(file);
        if (debugSource == NULL) return; // Failed to open file
    }

    markerHandle = debugSource->markerAdd(line-1, QsciScintilla::RightArrow);
}

void Editor::debugClear()
{
    if (markerHandle != -1 && debugSource != NULL)
        debugSource->markerDeleteHandle(markerHandle);
}

void Editor::lock()
{
    int i;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        src->lock();
    }
}

void Editor::unlock()
{
    int i;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        src->unlock();
    }
}

QList<Breakpoint*> Editor::getBreakpoints()
{
    QList<Breakpoint*> breakpoints;

    int i;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        breakpoints.append(src->getBreakpoints());
    }

    return breakpoints;
}
