# LimLim - Simple Lua editor and debugger

LimLim is small simple but useful debugger for Lua programming language.

## Capabilities
 * Stepping
 * Variable introspection
 * Table debugging
 * Locals and globals view
 * Stack traceback
 * Executing from console with arguments
 * Specify paths to LimDebug module and LimDebug controller

## Dependancies
 * Qt 4.7.2
 * Lua 5.1
 * QScintilla 6.1.0
 
## Installation
 * qmake LimLim.pro
 * make (or mingw32-make for windows)

## Future work
 * Add QScintilla features.
    * find/replace
    * bind toolbar buttons to qscintilla
    * changing of appearance (monospace font)
 * project managment (file tree at side of LimLim)
 * remove " from field values of tables (not so easy as i thought)
 * add feature that user can change values of introspected values
 * add gui widget for breakpoints and watchpoints
 * debugging is slow (on windows is SLOW - i think doesn't have so quick process input/output managment like unix systems), program would need complete change of debugger 
    * i think saving Lua states to XML file and sending directly to Qt application would be ideal
    * lua debugger have to remeber, which values needs to send main program
    * main program would send complex orders to Lua debugger with executable lua codes or XML
