#ifndef SOURCE_H
#define SOURCE_H

#include <QWidget>
#include <QFile>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

#include "breakpoint.h"

// TODO add function lock() to prevent source editting when debugging
//      when executing program all sources will lock
//      locking includes file save

class Source : public QsciScintilla

{
Q_OBJECT
public:
    explicit Source(const QString& sourceFile = 0, QWidget *parent = 0);

    bool doesExist() const { return exist; }
    QString getName() const;
    QString getFileName() const { return file; }

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
