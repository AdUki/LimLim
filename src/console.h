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
    void emitInput(QByteArray input);
    void emitOutput(QByteArray output);
    void emitError(QByteArray error);

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
