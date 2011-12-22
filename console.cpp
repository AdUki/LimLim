#include "console.h"

Console::Console(QWidget *parent) : QTextEdit(parent)
{
    setReadOnly(true);
    setTextBackgroundColor(QColor::fromRgb(255,255,255,255));
}


QByteArray Console::readInput()
{
    return inputBuffer.readAll();
}

void Console::open()
{
    setReadOnly(false); clear();
    inputBuffer.open(QIODevice::Text | QIODevice::ReadWrite);
    errorBuffer.open(QIODevice::Text | QIODevice::ReadWrite);
    outputBuffer.open(QIODevice::Text | QIODevice::ReadWrite);
    command = "";
}

void Console::close()
{
    setReadOnly(true);
    inputBuffer.close();
    errorBuffer.close();
    outputBuffer.close();
}

void Console::writeOutput(QByteArray data)
{
    outputBuffer.write(data);
    setTextColor(QColor::fromRgb(0,200,0,255));
    insertPlainText(data);
}

void Console::writeError(QByteArray data)
{
    errorBuffer.write(data);
    setTextColor(QColor::fromRgb(200,0,0,255));
    insertPlainText(data);
}

void Console::confirmCommand()
{
    // TODO check if correct, maybe enconding conflict
    inputBuffer.write(command.toAscii());
    command = "";
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
        e->accept();
    }
    // Movement and copy operations
    else if (e->key() == Qt::Key_Left ||
        e->key() == Qt::Key_Right ||
        e->key() == Qt::Key_Up ||
        e->key() == Qt::Key_Down ||
        e->matches(QKeySequence::Copy))
    {
        e->accept();
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
            e->accept();
        }
    }
    // Input operations
    else {
        setTextColor(QColor::fromRgb(0,0,0,255));
           
        int selection_size = textCursor().selectedText().length();
        if (selection_size > 0) cursor_pos = textCursor().selectionStart();

        int command_pos = cursor_pos + command.length()
                - (document()->characterCount() - 1);

        if (command_pos >= 0)
        {
            if (selection_size > 0) command.remove(command_pos, selection_size);
            command.insert(command_pos, e->text());
            e->accept();
        }
    }
    e->ignore();
}
