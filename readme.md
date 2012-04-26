# LimLim - Simple lua editor and debugger

LimLim is small simple but useful debugger for Lua programming language.

## Capabilities
 * Stepping
 * Variable introspection
 * Table debugging
 * Locals and globals view
 * Stack traceback
 * Executing from console with arguments

## Dependancies
 * Qt 4.7.2
 * Lua 5.1
 * QScintilla 6.1.0
 
## Installation
 * qmake LimLim.pro
 * make (or mingw32-make for windows)

## Future work
 * Add QScintilla features.
    ** find/replace
    ** bind toolbar buttons to qscintilla
    ** changing of appearance (monospace font)
 * project managment (file tree at side of LimLim)
 * changing of program base path to file (and add this feature to Lua Options)
 * remove " from field values of tables (not so easy as i thought)
 * add feature that user can change values of introspected values
 * add gui widget for breakpoints and watchpoints
 * On windows debugging is SLOW. I suppose windows doesn't have so quick process input/output managment so that slows program down (or it is worse qtprocess implementation???). I would need complete change of LimDebug (i think saving Lua states to XML file and sending directly to Qt application would be ideal)
