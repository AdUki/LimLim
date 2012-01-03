#include "editor.h"

#include <QGridLayout>
#include <QFileDialog>

Editor::Editor(QWidget *parent) :
    QTabWidget(parent)
{
    setTabsClosable(true);
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabWithSource(int)));
}

const Source* Editor::newSource()
{
    return addSource(new Source(NULL, this));
}

const Source* Editor::openSource()
{
    QString filename = QFileDialog::getOpenFileName(this,
                tr("Open Lua file"), ".",
                tr("Lua files (*.lua)\n"
                   "All Files (*)"));

    return openSource(filename);
}

const Source* Editor::openSource(const QString &filename)
{
    if (filename.isEmpty()) return NULL;
    return addSource(new Source(filename, this));
}

const Source* Editor::addSource(Source* source)
{
    int i, tabIndex = -1;
    for (i = 0; i < count(); i++) {
        Source* src = static_cast<Source*>(widget(i));
        if (*src == *source) {
            tabIndex = i;
            break;
        }
    }
    if (tabIndex >= 0) {
        setCurrentIndex(tabIndex);
        setStatusTip(tr("Source already opened"));
    } else {
        QString sourceName = source->getName();
        if (sourceName.compare("") != 0) {
            int i = addTab(source, sourceName);
            setCurrentIndex(i);
            connect(source, SIGNAL(modificationChanged(bool)), this, SLOT(updateSourceNames()));
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
