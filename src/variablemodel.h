#ifndef VARIABLEMODEL_H
#define VARIABLEMODEL_H

#include "variable.h"
#include "debugger.h"

#include <QAbstractItemModel>

class VariableModel : public QAbstractItemModel
{
Q_OBJECT
public:
    explicit VariableModel(Debugger *debugger, QObject *parent = 0);
/*
    QVariant data(const QModelIndex &index, int role) const;
*/
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
/*
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
*/

public slots:
    void updateVariables();

private:
    QList<Variable*> variables;
};

#endif // VARIABLEMODEL_H
