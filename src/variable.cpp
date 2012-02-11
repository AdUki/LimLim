#include "variable.h"

Variable::Variable(Debugger* debugger,
                   const QString& name,
                   bool nameEditable, bool valueEditable)
    : editableName(nameEditable),
      editableValue(valueEditable)
{
    this->debugger = debugger;
    this->name = name;
    emit nameChanged(this->name);

    if (isNameValid(name)) update();
    else {
        value = tr("<<INVALID EXPRESSION>>");
        emit valueChanged(value);
    }

    connect(debugger, SIGNAL(updateVariables()), this, SLOT(update()));
}

void Variable::setName(const QString& name)
{
    if (!editableName) return; // TODO give info to user via status bar
    if (isNameValid(name)) {
        this->name = name;
        emit nameChanged(this->name);
    }
}

void Variable::setValue(const QString& value)
{
    if (!editableValue) return; // TODO give info to user via status bar
    if (isValueValid(value)) {
        this->value = value;
        emit valueChanged(this->value);
    }
}

const QString& Variable::getName() const
{
    return name;
}

const QString& Variable::getValue() const
{
    return value;
}

Variable::Type Variable::getType() const
{
    return type;
}

void Variable::setType(Type type)
{
    this->type = type;
    emit typeChanged(type);
}

void Variable::update()
{
    // connect with debugger
    // TODO warning! this may get tricky if other variable is still connected to debugger...
    connect(debugger, SIGNAL(commandOutput(QByteArray)),
            this,     SLOT(recieveResult(QByteArray)));

    // get type
    debugger->giveCommand(QByteArray().append(EvaluateCommand)
                          .append("type (").append(name).append(")\n"));

    // get value
    debugger->giveCommand(QByteArray().append(EvaluateCommand)
                          .append(name).append('\n'));
}

void Variable::recieveResult(const QByteArray &result)
{
    static int state = 0;
    Type oldType = type;
    switch (state) {

    case 0: // get type
        if (result.startsWith("nil"))       type = Nil;
        if (result.startsWith("number"))    type = Number;
        if (result.startsWith("string"))    type = String;
        if (result.startsWith("function"))  type = Function;
        if (result.startsWith("boolean"))   type = Boolean;
        if (result.startsWith("table"))     type = Table;
        if (result.startsWith("userdata"))  type = UserData;
        if (oldType != type) emit typeChanged(type);
        break;

    case 1: // get value
        if (value.compare(result) != 0) {
            value = result;
            emit valueChanged(value);
        }
        // disconnect from debugger
        disconnect(debugger, SIGNAL(commandOutput(QByteArray)),
                   this,     SLOT(recieveResult(QByteArray)));

        emit updateDone();
    }

    state = (state + 1) % 2;
}

bool Variable::isNameValid(const QString &name)
{
    return true;
}

bool Variable::isValueValid(const QString &value)
{
    return true;
}
