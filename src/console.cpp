#include "console.h"

#include <QApplication>
#include <QClipboard>

Console::Console(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
    setTextBackgroundColor(QColor::fromRgb(255,255,255));
    setLineWrapMode(QTextEdit::NoWrap);
    printOutput = true;
}

void Console::setSilent()
{
    printOutput = false;
}

void Console::setVerbose()
{
    printOutput = true;
}

void Console::open()
{
    setReadOnly(false); clear();
    errorBuffer.open(QIODevice::Text | QIODevice::ReadWrite);
    outputBuffer.open(QIODevice::Text | QIODevice::ReadWrite);
    command = "";
}

void Console::close()
{
    setReadOnly(true);
    errorBuffer.close();
    outputBuffer.close();
}

void Console::writeSystem(const QString& message)
{
    setTextColor(QColor::fromRgb(0,0,200));
    insertPlainText(message);
    ensureCursorVisible();
}

// TODO bind function to paste action
// TODO maybe this function must be executed in thread
void Console::writeInput(const QByteArray& data)
{
    setTextColor(QColor::fromRgb(0,0,0));
    QList<QByteArray> commands = data.split('\n');
    QList<QByteArray>::iterator iter = commands.begin();
    while (true) {
        if (printOutput) insertPlainText(*iter);
        command.append(*iter);
        iter++;
        if (iter == commands.end()) break;
        else {
            if (printOutput) insertPlainText("\n");
            confirmCommand();
        }
    }
    ensureCursorVisible();
}

void Console::writeOutput(const QByteArray& data)
{
    if (printOutput) {
        outputBuffer.write(data);
        setTextColor(QColor::fromRgb(0,200,0));
        insertPlainText(data);
        ensureCursorVisible();
    }
    emit emitOutput(data);
}

void Console::writeError(const QByteArray& data)
{
    if (printOutput) {
        errorBuffer.write(data);
        setTextColor(QColor::fromRgb(200,0,0));
        insertPlainText(data);
        ensureCursorVisible();
    }
    emit emitError(data);
}

void Console::confirmCommand()
{
    // TODO check if correct, maybe enconding conflict
    QByteArray input(command.toAscii().append('\n'));
    command = "";
    emit emitInput(input);
}

void Console::keyPressEvent ( QKeyEvent * e )
{
    // progamers chars from 33 - 126
    //  QApplication::keyboardModifiers

    unsigned cursor_pos = textCursor().position();

    // Enter operation
    if (e->key() == Qt::Key_Return ||
        e->key() == Qt::Key_Enter)
    {
        confirmCommand();
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
        QTextEdit::keyPressEvent(e);
    }
    // Movement and copy operations
    else if (e->key() == Qt::Key_Left ||
        e->key() == Qt::Key_Right ||
        e->key() == Qt::Key_Up ||
        e->key() == Qt::Key_Down ||
        e->matches(QKeySequence::Copy))
    {
        QTextEdit::keyPressEvent(e);
    }
    // Delete operations
    else if (e->key() == Qt::Key_Backspace ||
        e->key() == Qt::Key_Delete ||
        e->matches(QKeySequence::Cut))
    {
        int selection_size = textCursor().selectedText().length();
        if (selection_size == 0) {
            if (e->key() == Qt::Key_Backspace) cursor_pos--;
            selection_size = 1;
        } else {
            cursor_pos = textCursor().selectionStart();
        }

        int command_pos = cursor_pos + command.length()
                - (document()->characterCount() - 1);

        if (command_pos >= 0)
        {
            command.remove(command_pos, selection_size);
            QTextEdit::keyPressEvent(e);
        }
    }
    // Copy operation
    else if (e->matches(QKeySequence::Copy)) { // TODO doesn't catch key sequence
        QTextEdit::keyPressEvent(e);
    }
    // Paste operation
    else if (e->matches(QKeySequence::Paste)) {
        QClipboard *clipboard = QApplication::clipboard();
        writeInput(clipboard->text().toAscii());
    }
    // Input operations
    else {
        setTextColor(QColor::fromRgb(0,0,0));
           
        int selection_size = textCursor().selectedText().length();
        if (selection_size > 0) cursor_pos = textCursor().selectionStart();

        int command_pos = cursor_pos + command.length()
                - (document()->characterCount() - 1);

        if (command_pos >= 0)
        {
            if (selection_size > 0) command.remove(command_pos, selection_size);
            command.insert(command_pos, e->text());
            QTextEdit::keyPressEvent(e);
        }
    }
}
