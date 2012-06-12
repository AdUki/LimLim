#include "debugger.h"

#include <QTreeWidgetItem>
#include <QDebug>
#include <QDir>

static const QByteArray StartCommand = QByteArray("> ");
static const QByteArray RunCommand = QByteArray("run\n");
static const QByteArray StepOverCommand = QByteArray("over\n");
static const QByteArray StepIntoCommand = QByteArray("step\n");
static const QByteArray LocalCommand = QByteArray("local\n");
static const QByteArray ExecuteCommand = QByteArray("exec ");
static const QByteArray EvaluateCommand = QByteArray("eval ");
static const QByteArray TableCommand = QByteArray("table ");
static const QByteArray StackCommand = QByteArray("traceback\n");
static const QByteArray GlobalCommand = QByteArray("global\n");

static const QByteArray PauseMessage = QByteArray("Paused:");
static const QByteArray EvaluateMessage = QByteArray("Evaluate:");
static const QByteArray TableMessage = QByteArray("Table:");
static const QByteArray LocalMessage = QByteArray("Local:");
static const QByteArray GlobalMessage = QByteArray("Global:");
static const QByteArray StackMessage = QByteArray("\nstack traceback:");

/*
	Lua debugger class

	This debugger can be used remotely, whitout lua interpreter,
	to debbug embedded lua applications.
*/
Debugger::Debugger(Editor *editor, Console *console, QObject *parent) :
    QObject(parent)
{
    remdebug = new Interpreter(console);
    this->editor = editor;

    autoRun = false;
    updateLocals = true;
    updateGlobals = true;
    updateStack = true;
    status = Off;

    connect(console, SIGNAL(emitOutput(QByteArray)), this, SLOT(parseInput(QByteArray)));
    connect(remdebug, SIGNAL(changedRunningState(bool)), this, SLOT(stateChange(bool)));
    connect(editor, SIGNAL(breakpointSet(int,QString)), this, SLOT(breakpointSet(int,QString)));
    connect(editor, SIGNAL(breakpointDeleted(int,QString)), this, SLOT(breakpointDeleted(int,QString)));

    // set up console
    this->console = console;
}

Debugger::~Debugger()
{
    // End debugger if it is runnning
    if (status != Off) stop();
}

void Debugger::start()
{
    if (status != Off) return;
    // Start RemDebug controller
    QString contPath = QString("limdebug")
            .append(QDir::separator())
            .append("controller.lua");
    remdebug->runFile(contPath);
}

void Debugger::stop()
{
    if (status != Off) remdebug->kill();
}

void Debugger::stepOver()
{
    console->setSilent();
    giveCommand(StepOverCommand);
    if (updateLocals) giveCommand(LocalCommand);
    if (updateStack) giveCommand(StackCommand);
    if (updateGlobals) giveCommand(GlobalCommand);
}

void Debugger::stepIn()
{
    console->setSilent();
    giveCommand(StepIntoCommand);
    if (updateLocals) giveCommand(LocalCommand);
    if (updateStack) giveCommand(StackCommand);
    if (updateGlobals) giveCommand(GlobalCommand);
}

void Debugger::run()
{
    console->setSilent();
    giveCommand(RunCommand);
    if (updateLocals) giveCommand(LocalCommand);
    if (updateStack) giveCommand(StackCommand);
    if (updateGlobals) giveCommand(GlobalCommand);
}



inline void Debugger::giveCommand(const QByteArray& command)
{
    switch (status) {
    case Waiting:
        status = Running;
        console->writeInput(command);
        emit waitingForCommand(false);
        break;
    case Running:
        input.append(command);
        break;
    case On:
        break;
    case Off:
        break;
    }
}

void Debugger::parseInput(const QByteArray& remdebugOutput)
{
    output.append(remdebugOutput);

    // Return if RemDebug didn't write whole status
    if (!output.endsWith(StartCommand)) return;

    // Controller initialization
    if (status == On) {
        status = Running;

        // Lock editor for editing
        editor->lock();

        // Read breakpoints
        QList<Breakpoint*> breakpoints = editor->getBreakpoints();
        QList<Breakpoint*>::iterator iter = breakpoints.begin();

        for (; iter != breakpoints.end(); iter++) {
            Breakpoint *br = static_cast<Breakpoint*> (*iter);
            breakpointSet(br->line, br->file);
        }

        if (autoRun) giveCommand(RunCommand);
    }

    // Parse status messages from output
    if (output.startsWith(PauseMessage)) {
        QRegExp rx(QString("^").append(PauseMessage)
                   .append("( line )?(\\d*)( watch )?(\\d*) file (.*)\n")
                   .append(StartCommand)
                   .append('$'));
        rx.indexIn(output);

        int line = rx.cap(2).toInt();   // get line number
        int watch = rx.cap(4).toInt();  // get watch id
        Q_UNUSED(watch);
        QString file = rx.cap(5).trimmed(); // Damn you whitespace! Give me back my 3 hours!!!

        // TODO change to signal
        editor->debugLine(file, line);

        // Update all watched expressions
        emit luaStateChanged();

    // Parse value of watched expressions
    } else if (output.startsWith(EvaluateMessage)) {
        output.chop(StartCommand.length());

        QRegExp rx("\\s*(\\w+)\t(\\d+)\t");

        int pos = EvaluateMessage.length();
        while ((pos = rx.indexIn(output, pos)) != -1) {
            pos += rx.matchedLength();

            QRegExp valueRx(QString("(.{").append(rx.cap(2)).append("})"));
            pos = valueRx.indexIn(output, pos);
            pos += valueRx.matchedLength();

            if (!watches.isEmpty()) {
                QTreeWidgetItem *item = watches.takeFirst();    // take first watch from list
                item->setText(1, valueRx.cap(1));   // set watch value
                item->setText(2, rx.cap(1));        // set watch type

                // update table fields
                if (item->text(2).compare("table") == 0) {
                    if (item->childCount() > 0) updateTable(item);
                } else item->takeChildren();
            }
        }

    // Parse value of local variables
    } else if (output.startsWith(LocalMessage)) {
        output.chop(StartCommand.length());

        QList<QTreeWidgetItem*> locals;

        int pos = LocalMessage.length();
        QRegExp numRx("(\\d+)[\r\n]+");
        pos = numRx.indexIn(output, pos) + numRx.matchedLength();

        QRegExp rx("(\\d+)\t(\\d+)\t");
        while ((pos = rx.indexIn(output, pos)) != -1) {
            pos += rx.matchedLength();

            // parse values of field
            QRegExp fieldRx(QString("(.{")
                            .append(rx.cap(1))
                            .append("})\t(\\w+)\t(.{")
                            .append(rx.cap(2))
                            .append("})[\r\n]+"));
            pos = fieldRx.indexIn(output, pos);
            pos += fieldRx.matchedLength();

            QString name = fieldRx.cap(1);
            name = name.mid(1, name.length()-2);

            // create and add new field to table
            QTreeWidgetItem* newLocal = new QTreeWidgetItem((QTreeWidget*)0,
                QStringList() << name
                              << fieldRx.cap(3)   // value
                              << fieldRx.cap(2)); // type
            locals.append(newLocal);
        }
        emit localsChanged(&locals);

        // Parse value of global variables
        } else if (output.startsWith(GlobalMessage)) {
            output.chop(StartCommand.length());

            QList<QTreeWidgetItem*> globals;

            int pos = LocalMessage.length();
            QRegExp numRx("(\\d+)[\r\n]+");
            pos = numRx.indexIn(output, pos) + numRx.matchedLength();

            QRegExp rx("(\\d+)\t(\\d+)\t");
            while ((pos = rx.indexIn(output, pos)) != -1) {
                pos += rx.matchedLength();

                // parse values of field
                QRegExp fieldRx(QString("(.{")
                                .append(rx.cap(1))
                                .append("})\t(\\w+)\t(.{")
                                .append(rx.cap(2))
                                .append("})[\r\n]+"));
                pos = fieldRx.indexIn(output, pos);
                pos += fieldRx.matchedLength();

                QString name = fieldRx.cap(1);
                name = name.mid(1, name.length()-2);

                // create and add new field to table
                QTreeWidgetItem* newLocal = new QTreeWidgetItem((QTreeWidget*)0,
                    QStringList() << name
                                  << fieldRx.cap(3)   // value
                                  << fieldRx.cap(2)); // type
                globals.append(newLocal);
            }
            emit globalsChanged(&globals);

    // Parse stack trace
    } else if (output.startsWith(StackMessage)) {
        output.chop(StartCommand.length());

        QStringList stack;

        int pos = StackMessage.length();
        QRegExp rx("[\r\n]+\t([^\r\n]+)");
        while ((pos = rx.indexIn(output, pos)) != -1) {
            pos += rx.matchedLength();
            QString line = rx.cap(1);
            if (!line.isEmpty()) stack.append(line);
        }
        if (!stack.isEmpty()) stack.pop_back();
        emit stackChanged(&stack);

    // Parse values of table
    } else if(output.startsWith(TableMessage) && !tables.isEmpty()) {
        output.chop(StartCommand.length());

        // take first table from list
        QTreeWidgetItem *table = tables.takeFirst();

        // remeber expanded table names
        // TODO fix this for n nested tables
        expandedTables.clear();
        foreach(QTreeWidgetItem* field, table->takeChildren()) {
            if (field->childCount() > 0) expandedTables.insert(field->text(0));
        }

        int pos = TableMessage.length();
        QRegExp numRx("(\\d+)[\r\n]+");
        pos = numRx.indexIn(output, pos) + numRx.matchedLength();

        QRegExp rx("(\\d+)\t(\\d+)\t");
        while ((pos = rx.indexIn(output, pos)) != -1) {
            pos += rx.matchedLength();

            // parse values of field
            QRegExp fieldRx(QString("(.{")
                            .append(rx.cap(1))
                            .append("})\t(\\w+)\t(.{")
                            .append(rx.cap(2))
                            .append("})[\r\n]+"));
            pos = fieldRx.indexIn(output, pos);
            pos += fieldRx.matchedLength();

            // create and add new field to table
            QTreeWidgetItem* field = new QTreeWidgetItem(table,
                QStringList() << fieldRx.cap(1)   // field key
                              << fieldRx.cap(3)   // field value
                              << fieldRx.cap(2)); // field type
            table->addChild(field);

            // update nested table fields
            if (expandedTables.contains(field->text(0)) && field->text(2).compare("table") == 0)
                updateTable(field);
        }
        table->setExpanded(true);
    }

    output.clear();

    // Check if there is input waiting for controller
    if (input.isEmpty()) {
        status = Waiting;
        emit waitingForCommand(true);
    } else { // There is input, write as command to console.
        status = Running;

        static QBuffer buffer(&input);
        if (!buffer.isOpen()) buffer.open(QIODevice::ReadOnly | QIODevice::Text);

        console->writeInput(buffer.readLine());

        if (buffer.atEnd()) {
            buffer.close();
            input.clear();
        }
    }
}

void Debugger::stateChange(bool running)
{
    if (running) { // controller started
        status = On;
        emit started();
        emit updateActions(true);
    } else { // controller ended
        status = Off;
        output.clear();
        input.clear();
        watches.clear();
        editor->debugClear();
        editor->unlock();
        emit waitingForCommand(false);
        emit finished();
        emit updateActions(false);
        console->setVerbose();
    }
}

void Debugger::breakpointSet(int line, QString file)
{
    console->setSilent();
    giveCommand(QString("setb %1 %2\n").arg(file).arg(line).toAscii());
}

void Debugger::breakpointDeleted(int line, QString file)
{
    console->setSilent();
    giveCommand(QString("delb %1 %2\n").arg(file).arg(line).toAscii());
}

/** Adds new expression to watch list. Singal watchesUpdated is emmited
  * when expressions are newly evaluated.
  * When expression is already in the map, expression is not added but
  * updated.
  */
void Debugger::updateWatch(QTreeWidgetItem *watch)
{
    if (status == Off || status == On) return;
    console->setSilent();
    
    watches.append(watch);

    // evaluate watch
    giveCommand(QByteArray(EvaluateCommand)
        .append("type(")
        .append(getWatchName(watch))
        .append("), string.len(tostring(")
        .append(getWatchName(watch))
        .append(")), (")
        .append(getWatchName(watch))
        .append(")\n")
    );
}

void Debugger::updateWatches(QList<QTreeWidgetItem*> *watches)
{
    if (status == Off || status == On) return;
    console->setSilent();

    this->watches.append(*watches);

    QStringList eval(EvaluateCommand);

    foreach (QTreeWidgetItem *watch, *watches) {
        eval.append("type(");
        eval.append(getWatchName(watch));
        eval.append("), string.len(tostring(");
        eval.append(getWatchName(watch));
        eval.append(")), (");
        eval.append(getWatchName(watch));
        eval.append(")");
        eval.append(", ");
    }
    eval.removeLast();
    eval.append("\n");
    giveCommand(eval.join("").toAscii());
}

// TODO refactor QTreeWidgetItem to its own class and put this method there
QString Debugger::getWatchName(const QTreeWidgetItem *watch)
{
    QString rootTable = watch->text(0);
    QString tableIndexes = "";
    while (watch->parent() != NULL)
    {
        watch = watch->parent();
        rootTable.prepend('[');
        rootTable.append(']');

        tableIndexes.prepend(rootTable);
        rootTable = watch->text(0);
    }
    return rootTable.append(tableIndexes);
}

void Debugger::updateTable(QTreeWidgetItem *table)
{
    if (status == Off || status == On) return;
    console->setSilent();

    tables.append(table);

    giveCommand(QByteArray(TableCommand)
                .append(getWatchName(table))
                .append("\n"));
}

void Debugger::setWatch(QTreeWidgetItem *watch)
{
    //if (numUpdated > 0) { numUpdated--; return; }
    if (status == Off || status == On) return;
    console->setSilent();

    giveCommand(QByteArray(ExecuteCommand)
                .append(getWatchName(watch)
                .append(" = ")
                .append(watch->text(1)))
                .append("\n"));

    updateWatch(watch);
}
