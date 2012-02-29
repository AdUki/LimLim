#include "watcher.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QAction>

Watcher::Watcher(QWidget *parent) : QTreeView(parent)
{
    model = new QStandardItemModel(5, 3, this);
    model->setHeaderData(0, Qt::Horizontal, QVariant(tr("Expression")), Qt::DisplayRole);
    model->setHeaderData(1, Qt::Horizontal, QVariant(tr("Value")), Qt::DisplayRole);
    model->setHeaderData(2, Qt::Horizontal, QVariant(tr("Type")), Qt::DisplayRole);
    setModel(model);

    setSelectionMode(QTreeView::SingleSelection);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,             SLOT(updateActions(QModelIndex)));
    connect(model, SIGNAL(itemChanged(QStandardItem*)),
            this,  SLOT(updateItem(QStandardItem*)));
}

void Watcher::updateActions(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);
}

void Watcher::updateItem(QStandardItem *item)
{
    static bool selfCall = false;
    if (selfCall) return;
    selfCall = true;

    QStandardItem *exp = model->item(item->row(), 0);
    QStandardItem *val = model->item(item->row(), 1);
    QStandardItem *type = model->item(item->row(), 2);

    switch(item->column()) {
    case 0: // expression
        setItemExpression(val, QString("eval ")
                          .append(item->data(Qt::EditRole).toString());

        setItemExpression(type, QString("eval type(")
                          .append(item->data(Qt::EditRole).toString().append(")")));

        emit updateWatch(val);
        emit updateWatch(type);
        break;
    case 1: // value
        // TODO here will user set values for wathces
        break;
    case 2: // type
        // You can't change type
        break;
    }

    selfCall = false;
}

void Watcher::updateAll()
{
    if (!isVisible()) return;

    // TODO check if in colums are also all table fields
    QList<QStandardItem*>::iterator i1 = model->takeColumn(1).begin();
    QList<QStandardItem*>::iterator i2 = model->takeColumn(2).begin();
    while (i1 != model->takeColumn(1).end()) {
        emit updateWatch(*i1);
        emit updateWatch(*i2);
        ++i1; ++i2;
    }
}

bool Watcher::setItemExpression(QStandardItem *item, const QString &exp)
{
    // TODO implement this method
    return false;
}
