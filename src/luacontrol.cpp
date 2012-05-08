#include <QtGui>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "console.h"
#include "luacontrol.h"
#include "interpreter.h"
#include "editor.h"
#include "debugger.h"
#include "watcher.h"
#include "stack.h"

LuaControl::LuaControl()
{
    // process arguments
    QStringList args = QCoreApplication::arguments();
    QString exec(args.takeFirst()); // take executable name
    QString file; // take file name
    if (!args.isEmpty()) file = args.takeFirst();

    // set up editor
    luaConsole = new Console(this);
    if (file.isEmpty()) luaEditor = new Editor(this);
    else luaEditor = new Editor(this, file);
    setCentralWidget(luaEditor);

    // set up interpreter
    luaInterpret = new Interpreter(luaConsole, this);
    luaInterpret->addArgs(args);

    // set up debugger
    debugConsole = new Console(this);
    luaDebugger = new Debugger(luaEditor, debugConsole, this);
    connect(luaDebugger,  SIGNAL(started()),
            this,           SLOT(controllerReady()));
    connect(luaDebugger, SIGNAL(waitingForCommand(bool)),
            debugConsole, SLOT(setPrintOutput(bool)));

    // set up other things
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createWatchers();
    createDockWindows();

    readSettings();
    setWindowIcon(QIcon(":/images/lua.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    this->setDockNestingEnabled(true);
}

void LuaControl::run()
{
    // execute Lua program
    luaInterpret->run(luaEditor->currentSource());
}

void LuaControl::debug()
{
    if (luaEditor->currentSource() == NULL) return;
    debugConsole->setVerbose();
    if (luaDebugger->getStatus() == Debugger::Off) {
        debugging = true;
        luaDebugger->start();
    } else luaInterpret->debug(luaEditor->currentSource());
}

void LuaControl::stop()
{
    luaDebugger->stop();
    luaInterpret->kill();
}

void LuaControl::controllerReady()
{
    if (debugging) luaInterpret->debug(luaEditor->currentSource());
    debugging = false;
}


void LuaControl::createActions()
{
    // NEW FILE ACTIION
    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(QIcon(":/images/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new file"));
    connect(newAction, SIGNAL(triggered()), luaEditor, SLOT(newSource()));

    // OPEN RECENT FILE ACTIONS
    for (int i = 0; i < MaxRecentFiles; i++) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                        this, SLOT(openRecentFile()));
    }

    // OPEN FILE ACTION
    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open new file"));
    connect(openAction, SIGNAL(triggered()), luaEditor, SLOT(openSource()));

    // SAVE FILE ACTION
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the Lua file to disk"));
    connect(saveAction, SIGNAL(triggered()), luaEditor, SLOT(saveCurrentSource()));

    // SAVE AS FILE ACTION
    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setStatusTip(tr("Save the Lua file under a new name"));
    connect(saveAsAction, SIGNAL(triggered()), luaEditor, SLOT(saveCurrentSourceAs()));

    // EXIT ACTION
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // CUT ACTION
    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setIcon(QIcon(":/images/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    //	connect(cutAction, SIGNAL(triggered()), editor, SLOT(cut()));

    // COPY ACTION
    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/images/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    //	connect(copyAction, SIGNAL(triggered()), editor, SLOT(copy()));

    // PASTE ACTION
    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon(":/images/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    //	connect(pasteAction, SIGNAL(triggered()), editor, SLOT(paste()));

    // DELETE ACTION
    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("Delete the current selection's contents"));
    //	connect(deleteAction, SIGNAL(triggered()), editor, SLOT(removeSelectedText()));

    // ABOUT ACTION
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    // ABOUT QT ACTION
    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // RUN ACTION
    runAction = new QAction(tr("&Run"), this);
    runAction->setShortcut(Qt::Key_F5);
    runAction->setIcon(QIcon(":/images/run.png"));
    runAction->setStatusTip(tr("Run current chunk of Lua code"));
    connect(runAction, SIGNAL(triggered()), this, SLOT(run()));
    connect(luaInterpret, SIGNAL(changedRunningState(bool)), runAction, SLOT(setDisabled(bool)));


    // DEBUG ACTION
    debugAction = new QAction(tr("&Debug"), this);
    debugAction->setIcon(QIcon(":/images/debug.png"));
    debugAction->setStatusTip(tr("Debug current chunk of Lua code"));
    connect(debugAction, SIGNAL(triggered()), this, SLOT(debug()));
    connect(luaInterpret, SIGNAL(changedRunningState(bool)), debugAction, SLOT(setDisabled(bool)));

    // STOP ACTION
    stopAction = new QAction(tr("&Stop"), this);
    stopAction->setIcon(QIcon(":/images/process-stop.png"));
    stopAction->setStatusTip(tr("Stop current running program"));
    stopAction->setDisabled(true);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    connect(luaInterpret, SIGNAL(changedRunningState(bool)), stopAction, SLOT(setEnabled(bool)));

    // CONTINUE ACTION
    continueAction = new QAction(tr("&Continue"), this);
    continueAction->setIcon(QIcon(":/images/debug_run.png"));
    continueAction->setStatusTip(tr("Continue running program"));
    continueAction->setEnabled(false);
    connect(continueAction, SIGNAL(triggered()), luaDebugger, SLOT(run()));
    connect(luaDebugger, SIGNAL(waitingForCommand(bool)), continueAction, SLOT(setEnabled(bool)));

    // STEP INTO ACTION
    stepIntoAction = new QAction(tr("&Step into"), this);
    stepIntoAction->setIcon(QIcon(":/images/debug_into.png"));
    stepIntoAction->setStatusTip(tr("Step into function"));
    stepIntoAction->setEnabled(false);
    connect(stepIntoAction, SIGNAL(triggered()), luaDebugger, SLOT(stepIn()));
    connect(luaDebugger, SIGNAL(waitingForCommand(bool)), stepIntoAction, SLOT(setEnabled(bool)));

    // STEP OVER ACTION
    stepOverAction = new QAction(tr("Step &over"), this);
    stepOverAction->setIcon(QIcon(":/images/debug_next.png"));
    stepOverAction->setStatusTip(tr("Step over function"));
    stepOverAction->setEnabled(false);
    connect(stepOverAction, SIGNAL(triggered()), luaDebugger, SLOT(stepOver()));
    connect(luaDebugger, SIGNAL(waitingForCommand(bool)), stepOverAction, SLOT(setEnabled(bool)));

    // INTERPRETER ACTION
    interpretAction = new QAction(tr("&Lua options"), this);
    interpretAction->setEnabled(true);
    connect(interpretAction, SIGNAL(triggered()), luaInterpret, SLOT(show()));

    // START CONTROLLER ACTION
    startControlAction = new QAction(tr("Start controller"), this);
    startControlAction->setIcon(QIcon(":/images/bug_green.png"));
    startControlAction->setEnabled(true);
    connect(startControlAction, SIGNAL(triggered()), luaDebugger, SLOT(start()));
    connect(luaDebugger, SIGNAL(updateActions(bool)), startControlAction, SLOT(setDisabled(bool)));

    // STOP CONTROLLER ACTION
    stopControlAction = new QAction(tr("Stop controller"), this);
    stopControlAction->setIcon(QIcon(":/images/bug_red.png"));
    stopControlAction->setEnabled(true);
    stopControlAction->setDisabled(true);
    connect(stopControlAction, SIGNAL(triggered()), luaDebugger, SLOT(stop()));
    connect(luaDebugger, SIGNAL(updateActions(bool)), stopControlAction, SLOT(setEnabled(bool)));

}

void LuaControl::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);

    projectMenu = menuBar()->addMenu(tr("&Project"));
    projectMenu->addAction(runAction);
    projectMenu->addAction(debugAction);
    projectMenu->addAction(stopAction);
    projectMenu->addAction(interpretAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void LuaControl::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->setObjectName("filetoolbar");
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    runToolBar = addToolBar(tr("&Run"));
    runToolBar->setObjectName("runtoolbar");
    runToolBar->addAction(runAction);
    runToolBar->addAction(debugAction);
    runToolBar->addAction(stopAction);

    debugToolBar = addToolBar(tr("&Debug"));
    debugToolBar->setObjectName("debugtoolbar");
    debugToolBar->addAction(continueAction);
    debugToolBar->addAction(stepIntoAction);
    debugToolBar->addAction(stepOverAction);

    controllerToolBar = addToolBar("&Controller");
    controllerToolBar->setObjectName("controllertoolbar");
    controllerToolBar->addAction(startControlAction);
    controllerToolBar->addAction(stopControlAction);
}

void LuaControl::createStatusBar()
{
    statusLabel = new QLabel("Welcome to Lua IDE");
    statusBar()->addWidget(statusLabel);
    updateStatusBar();
}
void LuaControl::updateStatusBar()
{
}

void LuaControl::createDockWindows()
{
    QDockWidget *dock;

    // Lua interpreter dock widget
    dock = new QDockWidget(tr("Output"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(luaConsole);
    dock->setObjectName("interpreter");
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    // Watches dock widget
    dock = new QDockWidget(tr("Watches"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(luaWatchesView);
    dock->setObjectName("watches");
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // Locals dock widget
    dock = new QDockWidget(tr("Locals"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(luaLocalsView);
    dock->setObjectName("locals");
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // Locals dock widget
    dock = new QDockWidget(tr("Globals"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(luaGlobalsView);
    dock->setObjectName("globals");
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // Stack dock widget
    dock = new QDockWidget(tr("Stack traceback"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(luaStack);
    dock->setObjectName("stack");
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    // Controller dock widget for debug
    dock = new QDockWidget(tr("LimDebug"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setContentsMargins(0,0,0,0);
    dock->setWidget(debugConsole);
    dock->setObjectName("limdebug");
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    // Breakpoints dock widget
    // TODO implement breakpoint managment
}

void LuaControl::createWatchers()
{
    //
    // Editable watcher
    //
    luaWatchesView  = new Watcher(this);

    luaWatchesView->addItem();

    QAction *action;

    action = new QAction(tr("New watch"), luaWatchesView);
    connect(action, SIGNAL(triggered()), luaWatchesView, SLOT(addWatch()));
    luaWatchesView->addAction(action);

    // TODO shortcut doesn't work
    action = new QAction(tr("Delete watch"), luaWatchesView);
    action->setShortcut(QKeySequence::Delete);
    connect(action, SIGNAL(triggered()), luaWatchesView, SLOT(deleteWatch()));
    luaWatchesView->addAction(action);

    action = new QAction(tr("Clear all watches"), luaWatchesView);
    connect(action, SIGNAL(triggered()), luaWatchesView, SLOT(clearAllWatches()));
    luaWatchesView->addAction(action);
    
    connect(luaDebugger,  SIGNAL(luaStateChanged()),
            luaWatchesView, SLOT(updateAll()));
    connect(luaWatchesView, SIGNAL(updateWatch(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateWatch(QTreeWidgetItem*)));
    connect(luaWatchesView, SIGNAL(updateWatches(QList<QTreeWidgetItem*>*)),
            luaDebugger,      SLOT(updateWatches(QList<QTreeWidgetItem*>*)));
    connect(luaWatchesView, SIGNAL(updateTable(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateTable(QTreeWidgetItem*)));
    connect(luaWatchesView, SIGNAL(setWatch(QTreeWidgetItem*)),
            luaDebugger,      SLOT(setWatch(QTreeWidgetItem*)));

    //
    // Local variables watcher
    //
    luaLocalsView = new Watcher(this);
/*
    HideEventWatcher *ew = new HideEventWatcher(luaLocalsView);
    connect(ew, SIGNAL(isShown(bool)), luaDebugger, SLOT(setUpdateLocals(bool)));
    luaLocalsView->installEventFilter(ew);
*/
    connect(luaDebugger,  SIGNAL(localsChanged(QList<QTreeWidgetItem*>*)),
            luaLocalsView, SLOT(replaceAllWatches(QList<QTreeWidgetItem*>*)));
    connect(luaLocalsView, SIGNAL(updateWatch(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateWatch(QTreeWidgetItem*)));
    connect(luaLocalsView, SIGNAL(updateWatches(QList<QTreeWidgetItem*>*)),
            luaDebugger,      SLOT(updateWatches(QList<QTreeWidgetItem*>*)));
    connect(luaLocalsView, SIGNAL(updateTable(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateTable(QTreeWidgetItem*)));

    //
    // Global variables watcher
    //
    luaGlobalsView = new Watcher(this);
/*
    ew = new HideEventWatcher(luaGlobalsView);
    connect(ew, SIGNAL(isShown(bool)), luaDebugger, SLOT(setUpdateGlobals(bool)));
    luaGlobalsView->installEventFilter(ew);
*/
    connect(luaDebugger,  SIGNAL(globalsChanged(QList<QTreeWidgetItem*>*)),
            luaGlobalsView, SLOT(replaceAllWatches(QList<QTreeWidgetItem*>*)));
    connect(luaGlobalsView, SIGNAL(updateWatch(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateWatch(QTreeWidgetItem*)));
    connect(luaGlobalsView, SIGNAL(updateWatches(QList<QTreeWidgetItem*>*)),
            luaDebugger,      SLOT(updateWatches(QList<QTreeWidgetItem*>*)));
    connect(luaGlobalsView, SIGNAL(updateTable(QTreeWidgetItem*)),
            luaDebugger,      SLOT(updateTable(QTreeWidgetItem*)));

    //
    // Stack watcher
    //
    luaStack = new Stack(this);
    connect(luaDebugger, SIGNAL(stackChanged(QStringList*)),
            luaStack,      SLOT(setStack(QStringList*)));
    connect(luaStack, SIGNAL(highlightSource(QString,uint)),
            luaEditor, SLOT(gotoLine(QString,uint)));
/*
    ew = new HideEventWatcher(luaStack);
    connect(ew, SIGNAL(isShown(bool)), luaDebugger, SLOT(setUpdateStack(bool)));
    luaStack->installEventFilter(ew);
*/
}

void LuaControl::closeEvent(QCloseEvent *event)
{
        if (luaEditor->closeAllSources()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void LuaControl::updateRecentFileActions()
{/*
	QMutableStringListIterator i(recentFiles);
	while (i.hasNext()) {
		if (!QFile::exists(i.next()))
			i.remove();
	}

	for (int j = 0; j < MaxRecentFiles; ++j) {
		if (j < recentFiles.count()) {
			QString text = tr("&%1 %2")
					.arg(j + 1)
					.arg(strippedName(recentFiles[j]));
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		} else {
			recentFileActions[j]->setVisible(false);
		}
	}
        separatorAction->setVisible(!recentFiles.isEmpty());*/
}

void LuaControl::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) luaEditor->openSource(action->data().toString());
}

void LuaControl::about()
{
	QMessageBox::about(this, tr("About Lua IDE"),
                        tr("<h2>LimLim</h2>"
                           "<p>Copyright &copy; 2011 Simon Mikuda STU FIIT"
			   "<p>Lua IDE is a small application "
			   "for debugging and programming "
			   "in Lua language."));
}

void LuaControl::writeSettings()
{
        QSettings settings("Simon Mikuda", "LimLim");
        settings.setValue("mainwindow/geometry", saveGeometry());
        settings.setValue("mainwindow/state", saveState());
        settings.setValue("recentFiles", recentFiles);
}

void LuaControl::readSettings()
{
        QSettings settings("Simon Mikuda", "LimLim");
        restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
        restoreState(settings.value("mainwindow/state").toByteArray());
	recentFiles = settings.value("recentFiles").toStringList();
	updateRecentFileActions();
}
