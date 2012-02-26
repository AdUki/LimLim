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
    setContextMenuPolicy(Qt::ActionsContextMenu);\

    connect(selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,             SLOT(updateActions(QModelIndex)));
}

void Watcher::updateActions(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);
}
