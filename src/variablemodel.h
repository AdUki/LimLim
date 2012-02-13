#ifndef VARIABLEMODEL_H
#define VARIABLEMODEL_H

#include "debugger.h"

#include <QAbstractItemModel>

class VariableModel : public QAbstractItemModel
{
Q_OBJECT
public:
    explicit VariableModel(Debugger *debugger, QObject *parent = 0);

    /* Items data handling */

    Qt::ItemFlags flags(const QModelIndex &index) const;

    // Read-only access
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    // Editable items
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

    // Resizable models
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

    // Lazy Population of Model Data
    bool hasChildren(const QModelIndex &parent) const;

    /* Navigation and Model Index Creation */

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

public slots:
    void updateVariables();

private:

    class Variable
    {

    public:
        Variable(const QString& name,
                 bool nameEditable = false,
                 bool valueEditable = true)
            : editableName(nameEditable),
              editableValue(valueEditable)
        {
            this->name = name;
            this->parent = NULL;
        }

        const bool editableName;
        const bool editableValue;

        QString name;

        QList<Variable*> children;
        Variable *parent;
    };

    Variable* getVariable(const QModelIndex &index) const;

    QList<Variable*> variables;

    Debugger *debugger;
};

#endif // VARIABLEMODEL_H
