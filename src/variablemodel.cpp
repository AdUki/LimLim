#include "variablemodel.h"

VariableModel::VariableModel(Debugger *debugger, QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant VariableModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Value");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags VariableModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid()) return Qt::ItemIsEnabled;

     Variable* var = static_cast<Variable*>(index.internalPointer());

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

void VariableModel::updateVariables()
{

}
