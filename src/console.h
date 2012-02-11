#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>
#include <QBuffer>
#include <QKeyEvent>

/*
  TODO refactor this class so that programer cannot have
       access to inherited members
*/
class Console : public QTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = 0);

signals:
    void emitInput (const QByteArray& input);
    void emitOutput(const QByteArray& output);
    void emitError (const QByteArray& error);

public slots:

    void open();
    void close();

    void writeInput (const QByteArray& data);
    void writeOutput(const QByteArray& data);
    void writeError (const QByteArray& data);
    void writeSystem(const QString& message);

protected:
    void keyPressEvent ( QKeyEvent * e );

private:
    QString command;
    void confirmCommand();

    QBuffer outputBuffer;
    QBuffer errorBuffer;
};

#endif // CONSOLE_H
