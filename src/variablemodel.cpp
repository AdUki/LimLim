#include "variablemodel.h"

VariableModel::VariableModel(Debugger *debugger, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->debugger = debugger;
}

Qt::ItemFlags VariableModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid()) return Qt::ItemIsEnabled;

     Variable* var = getVariable(index);

     // TODO add read only mode

     return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

QVariant VariableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole && role != Qt::EditRole) return QVariant();

    Variable *item = getVariable(index);

    // Get variable data by column
    switch (index.column()) {
    case 0: // name
        return item->name;
    case 1: // type
        // TODO expression musn't be variable
        return debugger->getWatchExp(
                QString("type (")
                .append(item->name)
                .append(")"));
    case 2: // value
        return debugger->getWatchExp(item->name);
    }
}

QVariant VariableModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Type");
        case 2:
            return tr("Value");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

int VariableModel::rowCount(const QModelIndex &parent) const
{
    Variable* var = getVariable(parent);

    return var->children.count();
}

int VariableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

bool VariableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Set data by column and by role
    switch (index.column()) {
    case 0: // name
        // delete watched expression from debugger
        // add new watched expression to debugger
        // emit update
        return false;
    case 1: // type
        // if editable
        // !!! must be variable !!!
        // clear wathced expression
        // set type to variable
        // emit update
        return false;
    case 2: // value
        // if editable
        // !!! must be variable !!!
        // set new value to variable
        // emit update
        return false;
    }
}

bool VariableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{

}

bool VariableModel::insertRows(int row, int count, const QModelIndex &parent)
{

}

bool VariableModel::removeRows(int row, int count, const QModelIndex &parent)
{

}

bool VariableModel::hasChildren(const QModelIndex &parent) const
{

}

QModelIndex VariableModel::index(int row, int column, const QModelIndex &parent) const
{

}

QModelIndex VariableModel::parent(const QModelIndex &child) const
{

}


void VariableModel::updateVariables()
{

}

VariableModel::Variable* VariableModel::getVariable(const QModelIndex &index) const
{
    if (index.isValid()) {
        Variable* item = static_cast<Variable*>(index.internalPointer());
        if (item) return item;
    }
    return this->variables.first();
}
