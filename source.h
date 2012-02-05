#ifndef SOURCE_H
#define SOURCE_H

#define MARK_RIGHTARROW 1
#define MARK_BREAKPOINT 4

#include <QWidget>
#include <QFile>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

#include "breakpoint.h"

class Source : public QsciScintilla

{
Q_OBJECT
public:
    explicit Source(const QString& sourceFile = 0, QWidget *parent = 0);

    bool doesExist() const { return exist; }
    QString getName() const;
    QString getFileName() const { return file; }

    void lock();
    void unlock();

    QList<Breakpoint*> getBreakpoints();

    bool operator==(const Source &rhs);

signals:
    /*
      These signals are useful when user is setting and deleting
      breakpoints during execution.

      Invariant: text of code user cannot change, number of lines
          is important. Simply code is locked for editing during
          debugging.
    */
    void breakpointSet(int line, QString file);
    void breakpointDeleted(int line, QString file);

public slots:
    bool save();
    bool saveAs();
    bool canClose();

private slots:
    void marginBreakpoint(int marigin, int line, Qt::KeyboardModifiers state);
    void linesChanged();

private:
    enum {LineMargin, DebugMargin};

    void initScintilla();
    bool saveFile(const QString& file);

    QString file;
    bool exist;
};

#endif // SOURCE_H
