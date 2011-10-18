#include "luaeditor.h"

LuaEditor::LuaEditor(QWidget *parent) :
    QsciScintilla(parent)
{
	lexer = new QsciLexerLua(this);
	setLexer(lexer);

	setMarginType(1, NumberMargin);
	setMarginWidth(1, 40);

	setUtf8(true);
}
