#include "watcher.h"

#include <QAction>

Watcher::Watcher(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderItem(new QTreeWidgetItem(QStringList()
                                      << tr("Expression")
                                      << tr("Value")
                                      << tr("Type")));

    setSelectionMode(QTreeView::SingleSelection);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // TODO add actions for adding itmes
    this->addItem();
    // TODO add actions for deleting items

    connect(this, SIGNAL(itemSelectionChanged()),
            this,   SLOT(updateActions()));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,   SLOT(updateItem(QTreeWidgetItem*, int)));
}

void Watcher::updateActions()
{

}

void Watcher::updateItem(QTreeWidgetItem *item, int column)
{
    switch(column) {
    case 0: // expression
        emit updateWatch(item);
        break;
    case 1: // value
        // TODO here will user set values for wathces
        break;
    case 2: // type
        // You can't change type
        break;
    }
}

void Watcher::updateAll()
{
    for (int i = 0; i < topLevelItemCount(); ++i) {
        emit updateWatch(topLevelItem(i));
    }
}

void Watcher::addItem()
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem(this);
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    addTopLevelItem(newItem);
}

void Watcher::removeSelectedItems()
{
    QList<QTreeWidgetItem*>::iterator i;
    QList<QTreeWidgetItem*> list = selectedItems();

    for (i = list.begin(); i != list.end(); ++i) delete *i;
}
