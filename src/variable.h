#ifndef VARIABLES_H
#define VARIABLES_H

#include <QObject>

#include "debugger.h"

class Variable : public QObject
{
Q_OBJECT

public:
    Variable(Debugger* debugger, const QString& name,
             bool nameEditable = false,
             bool valueEditable = true);

    const bool editableName;
    const bool editableValue;

    enum Type {
        Nil,
        Boolean,
        Number,
        String,
        Function,
        Table,
        UserData
    };

              void setName(const QString& name);
    const QString& getName() const;
              void setValue(const QString& value);
    const QString& getValue() const;
              void setType(Type type);
              Type getType() const;

signals:
    void updateDone();

    void nameChanged(const QString& name);
    void valueChanged(const QString& value);
    void typeChanged(Type type);


public slots:
    void update();

private slots:
    void recieveResult(const QByteArray& result);

private:
    Type type;

    QString name;
    QString value;

    Debugger *debugger;

    QList<Variable*> children;
    Variable *parent;

    // These function will popup error message if invalid
    bool isNameValid(const QString& name);
    bool isValueValid(const QString& value);
};

#endif // VARIABLES_H
