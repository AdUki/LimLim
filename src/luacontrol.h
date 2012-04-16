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
class Watcher;
class Stack;

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
    void controllerReady();

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
    bool debugging;

    Console *luaConsole;
    Console *debugConsole;
    Interpreter *luaInterpret;
    Editor *luaEditor;
    Debugger *luaDebugger;
    Stack *luaStack;

    Watcher *luaWatchesView;
    Watcher *luaLocalsView;
    Watcher *luaGlobalsView;

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
    QToolBar *controllerToolBar;

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

    QAction *interpretAction;

    QAction *startControlAction;
    QAction *stopControlAction;
};

#endif // MAINWINDOW_H
