#include "source.h"

#include <QFileDialog>
#include <QTemporaryFile>
#include <QMessageBox>

Source::Source(const QString& sourceFile, QWidget *parent) :
        QsciScintilla(parent)
{
    initScintilla();

    if (sourceFile == NULL) exist = false;
    else {
        exist = true;
        QFile* source = new QFile(sourceFile, this);
        if (source->open(QIODevice::ReadOnly) && read(source)) {
            source->close();
            setModified(false);
            file = sourceFile;
        } else { // TODO rewrite to QErrorMessage
            QMessageBox::critical(this, tr("Error!"),
                tr("Cannot read from file!\n"
                   "Check if you have permissions to view this file"),
                QMessageBox::Ok);
        }
    }

    connect(this, SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),
            this, SLOT(marginBreakpoint(int,int,Qt::KeyboardModifiers)));
    connect(this, SIGNAL(linesChanged()), this, SLOT(linesChanged()));
}

void Source::initScintilla()
{
    // Lexer setup
    QsciLexer* lexer = new QsciLexerLua(this);
    setLexer(lexer);
    setUtf8(true);
    setColor(QColor::fromRgb(255,255,255,255));

    // Marker setup
    markerDefine('>', RightArrow);
    markerDefine('B', Circle);

    // Margin setup
    // TODO it is weird but it works
    setMarginType(LineMargin, TextMargin);
    setMarginSensitivity(LineMargin, true);
    setMarginLineNumbers(LineMargin, true);
    setMarginSensitivity(LineMargin, true);

    setMarginType(DebugMargin, NumberMargin);
    setMarginLineNumbers(DebugMargin, false);
    setMarginSensitivity(DebugMargin, true);

    linesChanged();
}

QString Source::getName() const
{
    QString shortName;

    if (exist) shortName.append(QFileInfo(file).fileName());
    else shortName.append(QString(tr("untitled")));
    if (isModified()) shortName.append('*');

    return shortName;
}

bool Source::operator==(const Source &rhs) {
    if (this == &rhs) return true;
    if (!this->exist || !rhs.exist) return false;
    if (QFileInfo(file) == QFileInfo(rhs.file)) return true;
    return false;
  }

bool Source::save()
{
    if (exist) return saveFile(file);
    else return saveAs();
}

bool Source::saveAs()
{
    QString newFile = QFileDialog::
        getSaveFileName(this, tr("Save as..."), QString(),
        tr("Lua-Files (*.lua);;All Files (*)"));

    if (saveFile(newFile)) { // saving ok
        file = newFile;
        return true;
    } else { // error in saving
        return false;
    }
}

// TODO add file name to the text
bool Source::saveFile(const QString &file)
{
    QFile source(file);
    forever {
        if (source.open(QIODevice::WriteOnly) && write(&source)) {
            source.close();
            setModified(false);
            exist = true;
            return true;
        }
        int r = QMessageBox::warning(this, tr("Lua IDE"),
             tr("Error in saving file!.\n"
                "What do you want to do?"),
             QMessageBox::Retry | QMessageBox::Discard | QMessageBox::Cancel);

        if (r == QMessageBox::Discard) return true;
        if (r == QMessageBox::Cancel) return false;
    }
}

// TODO add file name to the text
bool Source::canClose()
{
    if(isModified()) {
        int r = QMessageBox::warning(this, tr("Lua IDE"),
             tr("The document has been modified.\n"
                    "Do you want to save your changes?"),
             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (r == QMessageBox::Save) return save();
        if (r == QMessageBox::Cancel) return false;
    }
    return true;
}

void Source::marginBreakpoint(int margin, int line, Qt::KeyboardModifiers state)
{
    switch (margin) {
    case DebugMargin:
        unsigned mark = markersAtLine(line);
        if (mark | Circle) {
            markerDelete(line, markerFindNext(line, Circle));
            emit breakpointDeleted(line, getFileName());
        } else {
            markerAdd(line, Circle);
            emit breakpointSet(line, getFileName());
        }
        break;
    }
}

void Source::linesChanged()
{
    setMarginWidth(LineMargin, QString::number(lines()) + 1);
}

void Source::lock()
{
    lexer()->setPaper(QColor::fromRgb(250,250,250), -1);
    setReadOnly(true);
}

void Source::unlock()
{
    lexer()->setPaper(QColor::fromRgb(255,255,255), -1);
    setReadOnly(false);
}
