#include "watchmodel.h"

#include <QTreeView>

WatchModel::WatchModel(QTreeView *viewer, QObject *parent, bool locked)
    : QStandardItemModel(0, 3, parent)
{
    this->locked = locked;

    setHeaderData(0, Qt::Horizontal, QVariant(tr("Expression")), Qt::DisplayRole);
    setHeaderData(1, Qt::Horizontal, QVariant(tr("Value")), Qt::DisplayRole);
    setHeaderData(2, Qt::Horizontal, QVariant(tr("Type")), Qt::DisplayRole);

    viewer->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(viewer, SIGNAL(clicked(QModelIndex)),
            this,   SLOT(itemClicked(QModelIndex)));
    connect(viewer, SIGNAL(doubleClicked(QModelIndex)),
            this,   SLOT(itemDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(itemChanged(QStandardItem*)),
            this, SIGNAL(updateItem(QStandardItem*)));
}

void WatchModel::itemClicked(QModelIndex index)
{

}

void WatchModel::itemDoubleClicked(QModelIndex index)
{

}
