#include <QtGui>

#include "luacontrol.h"
#include "luainterpret.h"
#include "luaeditor.h"

LuaControl::LuaControl()
{
	interpret = new LuaInterpret(this);
	editor = new LuaEditor(this);
	setCentralWidget(editor);

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();

	readSettings();

	setWindowIcon(QIcon(":/images/lua.png"));
	setCurrentFile("");

	setAttribute(Qt::WA_DeleteOnClose);
}

void LuaControl::run()
{
	runAction->setEnabled(false);
	debugAction->setEnabled(false);
	interpret->setReadOnly(false);
	interpret->run(editor->text());
}

void LuaControl::debug()
{
	runAction->setEnabled(false);
	debugAction->setEnabled(false);
	interpret->setReadOnly(false);
	interpret->debug(editor->text());
}

void LuaControl::stop()
{
	interpret->stop();
}


void LuaControl::createActions()
{
	newAction = new QAction(tr("&New"), this);
	newAction->setIcon(QIcon(":/images/new.png"));
	newAction->setShortcut(QKeySequence::New);
	newAction->setStatusTip(tr("Create a new file"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	for (int i = 0; i < MaxRecentFiles; i++) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);;
		connect(recentFileActions[i], SIGNAL(triggered()),
				this, SLOT(openRecentFile()));
	}

	openAction = new QAction(tr("&Open"), this);
	openAction->setIcon(QIcon(":/images/open.png"));
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open new file"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	saveAction = new QAction(tr("&Save"), this);
	saveAction->setIcon(QIcon(":/images/save.png"));
	saveAction->setShortcut(QKeySequence::Save);
	saveAction->setStatusTip(tr("Save the Lua file to disk"));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAction = new QAction(tr("Save &As..."), this);
	saveAsAction->setStatusTip(tr("Save the Lua file under a new name"));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit the application"));
	connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

	cutAction = new QAction(tr("Cu&t"), this);
	cutAction->setIcon(QIcon(":/images/cut.png"));
	cutAction->setShortcut(QKeySequence::Cut);
	cutAction->setStatusTip(tr("Cut the current selection's contents "
							   "to the clipboard"));
	connect(cutAction, SIGNAL(triggered()), editor, SLOT(cut()));

	copyAction = new QAction(tr("&Copy"), this);
	copyAction->setIcon(QIcon(":/images/copy.png"));
	copyAction->setShortcut(QKeySequence::Copy);
	copyAction->setStatusTip(tr("Copy the current selection's contents "
								"to the clipboard"));
	connect(copyAction, SIGNAL(triggered()), editor, SLOT(copy()));

	pasteAction = new QAction(tr("&Paste"), this);
	pasteAction->setIcon(QIcon(":/images/paste.png"));
	pasteAction->setShortcut(QKeySequence::Paste);
	pasteAction->setStatusTip(tr("Paste the clipboard's contents into "
								 "the current selection"));
	connect(pasteAction, SIGNAL(triggered()), editor, SLOT(paste()));

	deleteAction = new QAction(tr("&Delete"), this);
	deleteAction->setShortcut(QKeySequence::Delete);
	deleteAction->setStatusTip(tr("Delete the current selection's contents"));
	connect(deleteAction, SIGNAL(triggered()), editor, SLOT(removeSelectedText()));

	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setStatusTip(tr("Show the application's About box"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAction = new QAction(tr("About &Qt"), this);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	runAction = new QAction(tr("&Run"), this);
	runAction->setStatusTip(tr("Run current chunk of Lua code"));
	connect(runAction, SIGNAL(triggered()), this, SLOT(run()));
	connect(interpret, SIGNAL(finished(bool)), runAction, SLOT(setEnabled(bool)));

	debugAction = new QAction(tr("&Debug"), this);
	debugAction->setStatusTip(tr("Debug current chunk of Lua code"));
	connect(debugAction, SIGNAL(triggered()), this, SLOT(debug()));
	connect(interpret, SIGNAL(finished(bool)), debugAction, SLOT(setEnabled(bool)));

	stopAction = new QAction(tr("&Stop"), this);
	stopAction->setStatusTip(tr("Stop current running program"));
	connect(stopAction, SIGNAL(triggered()), interpret, SLOT(stop()));
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

	executeMenu = menuBar()->addMenu(tr("E&xecute"));
	executeMenu->addAction(runAction);
	executeMenu->addAction(debugAction);
	executeMenu->addAction(stopAction);

	optionsMenu = menuBar()->addMenu(tr("&Options"));

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(aboutQtAction);
}

void LuaControl::createToolBars()
{
	fileToolBar = addToolBar(tr("&File"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);
	editToolBar = addToolBar(tr("&Edit"));
	editToolBar->addAction(cutAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);
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
	// Lua interpreter dock widget
	QDockWidget *dock = new QDockWidget(tr("Output"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setWidget(interpret);
	addDockWidget(Qt::BottomDockWidgetArea, dock);

	// Locals dock widget

	// Globals dock widget

	// Breakpoints dock widget

	// Watches dock widget
}

bool LuaControl::okToContinue()
{
	if (!editor->isModified()) return true;

	int r = QMessageBox::warning(this, tr("Lua IDE"),
								 tr("The document has been modified.\n"
									"Do you want to save your changes?"),
								 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

	if (r == QMessageBox::Save) return save();
	else if (r == QMessageBox::Cancel) return false;
	return true;
}

void LuaControl::newFile()
{
	if (okToContinue()) {
		editor->clear();
		editor->setModified(false);
		setCurrentFile("");
	}
}
void LuaControl::open()
{
	if (okToContinue()) {
		QString fileName = QFileDialog::getOpenFileName(this,
														tr("Open Lua file"), ".",
														tr("Lua files (*.lua)\n"
														   "All Files (*)"));
		if (!fileName.isEmpty())
			loadFile(fileName);
	}
}
bool LuaControl::loadFile(const QString &fileName)
{
	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly)) {
		statusBar()->showMessage(tr("Cannot open file for reading"));
		return false;
	}
	if (!editor->read(&file)) {
		statusBar()->showMessage(tr("Loading canceled"), 2000);
		return false;
	}
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
	file.close();
	return true;
}

bool LuaControl::save()
{
	if (curFile.isEmpty()) {
		return saveAs();
	} else {
		return saveFile(curFile);
	}
}
bool LuaControl::saveFile(const QString &fileName)
{
	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly)) {
		statusBar()->showMessage(tr("Cannot open file for writting"));
		return false;
	}
	if (!editor->write(&file)) {
		statusBar()->showMessage(tr("Saving canceled"), 2000);
		return false;
	}
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
	file.close();
	editor->setModified(false);
	return true;
}
bool LuaControl::saveAs()
{
	QString fn = QFileDialog::
			getSaveFileName(this, tr("Save as..."), QString(),
							tr("Lua-Files (*.lua);;All Files (*)"));

	if (fn.isEmpty()) return false;
	return saveFile(fn);
}

void LuaControl::closeEvent(QCloseEvent *event)
{
	if (okToContinue()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void LuaControl::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	setWindowModified(false);

	QString shownName = tr("Untitled");
	if (!curFile.isEmpty()) {
		shownName = strippedName(curFile);
		recentFiles.removeAll(curFile);
		recentFiles.prepend(curFile);
		updateRecentFileActions();
	}

	setWindowTitle(tr("%1[*] - %2").arg(shownName)
								   .arg(tr("Lua IDE")));
}
QString LuaControl::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void LuaControl::updateRecentFileActions()
{
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
	separatorAction->setVisible(!recentFiles.isEmpty());
}

void LuaControl::openRecentFile()
{
	if (okToContinue()) {
		QAction *action = qobject_cast<QAction *>(sender());
		if (action) {
			loadFile(action->data().toString());
			editor->setModified(false);
		}
	}
}

void LuaControl::about()
{
	QMessageBox::about(this, tr("About Lua IDE"),
			tr("<h2>Lua IDE 0.1</h2>"
			   "<p>Copyright &copy; 2011 STU FIIT"
			   "<p>Lua IDE is a small application "
			   "for debugging and programming "
			   "in Lua language."));
}

void LuaControl::writeSettings()
{
	QSettings settings("STU FIIL", "Lua IDE");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("recentFiles", recentFiles);
}

void LuaControl::readSettings()
{
	QSettings settings("STU FIIL", "Lua IDE");
	restoreGeometry(settings.value("geometry").toByteArray());
	recentFiles = settings.value("recentFiles").toStringList();
	updateRecentFileActions();
}
