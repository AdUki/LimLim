#ifndef SOURCE_H
#define SOURCE_H

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
    QString getShortFileName() const;

public slots:
    bool save();
    bool saveAs();
    bool canClose();

private:
    void initScintilla();
    bool saveFile(const QString& file);

    QString file;
    bool exist;
};

#endif // SOURCE_H
