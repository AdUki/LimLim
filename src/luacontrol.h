#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTextEdit;
class QLabel;
class QTreeView;
class QsciLexerLua;

class Debugger;
class Interpreter;
class Editor;
class Console;
class WatchModel;

class LuaControl : public QMainWindow
{
	Q_OBJECT

public:
	LuaControl();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void about();
	void openRecentFile();
	void updateStatusBar();
	void run();
	void debug();
	void stop();

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void createDockWindows();
        void createWatchers();

	void readSettings();
	void writeSettings();

	void updateRecentFileActions();

	QStringList recentFiles;

        Console *luaConsole;
        Console *debugConsole;
        Interpreter *luaInterpret;
        Editor *luaEditor;
        Debugger *luaDebugger;

        QTreeView *luaWatchesView;
        QTreeView *luaGlobalsView;
        QTreeView *luaLocalsView;

	QLabel *statusLabel;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *projectMenu;
	QMenu *viewMenu;
	QMenu *optionsMenu;
	QMenu *helpMenu;

	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *runToolBar;
        QToolBar *debugToolBar;

	enum { MaxRecentFiles = 5 };
	QAction *recentFileActions[MaxRecentFiles];
	QAction *separatorAction;

	QAction *newAction;
	QAction *openAction;
	QAction *saveAction;
	QAction *saveAsAction;
	QAction *aboutQtAction;
	QAction *aboutAction;
	QAction *exitAction;

	QAction *copyAction;
	QAction *pasteAction;
	QAction *cutAction;
	QAction *deleteAction;

        QAction *runAction;
	QAction *debugAction;
	QAction *stopAction;

        QAction *continueAction;
        QAction *stepOverAction;
        QAction *stepIntoAction;
};

#endif // MAINWINDOW_H
