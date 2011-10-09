#ifndef LUAEDITOR_H
#define LUAEDITOR_H

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

class LuaEditor : public QsciScintilla
{
    Q_OBJECT

public:
	explicit LuaEditor(QWidget *parent = 0);

private:
	QsciLexerLua *lexer;

signals:

public slots:

};

#endif // LUAEDITOR_H
