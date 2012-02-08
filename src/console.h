#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>
#include <QBuffer>
#include <QKeyEvent>


/** TODO
  * Console is for input and output. Write output is simple,
  * you just use QPlaingTextEdit append function. Writing
  * is handled by commandQueue. When user types something into
  * console and presses enter, typed line enqueues into command
  * queue. You can get commands by popCommand. With flushConsole
  * you will get console clear of all commands and text.
  */
class Console : public QTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = 0);

signals:
    void emitInput(QByteArray input);

public slots:

    void open();
    void close();

    void writeInput (QByteArray data);
    void writeOutput(QByteArray data);
    void writeError(QByteArray data);
    void writeSystem(QString message);

protected:
    void keyPressEvent ( QKeyEvent * e );

private:
    QString command;
    void confirmCommand();

    QBuffer outputBuffer;
    QBuffer errorBuffer;
};

#endif // CONSOLE_H
