#include "watcher.h"

#include <QAction>
#include <QScrollBar>

Watcher::Watcher(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderItem(new QTreeWidgetItem(QStringList()
        << tr("Expression") << tr("Value") << tr("Type")));

    setSelectionMode(QTreeView::SingleSelection);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    this->addItem();

    connect(this, SIGNAL(itemSelectionChanged()),
            this,   SLOT(updateActions()));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this,   SLOT(updateItem(QTreeWidgetItem*, int)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(expandTable(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(collapseTable(QTreeWidgetItem*)));

    QAction *action;

    action = new QAction(tr("New watch"), this);
    connect(action, SIGNAL(triggered()), SLOT(addWatch()));
    addAction(action);

    action = new QAction(tr("Delete watch"), this);
    action->setShortcut(QKeySequence::Delete);
    connect(action, SIGNAL(triggered()), this, SLOT(deleteWatch()));
    addAction(action);

    action = new QAction(tr("Clear all watches"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(clearAllWatches()));
    addAction(action);
}

void Watcher::updateActions()
{

}

void Watcher::addWatch()
{
    addItem();
}

void Watcher::deleteWatch()
{
    removeSelectedItems();
}

void Watcher::clearAllWatches()
{
    clear();
    addWatch();
}


void Watcher::updateItem(QTreeWidgetItem *item, int column)
{
    switch(column) {
    case 0: // expression
        if (!item->text(0).isEmpty()) {
            if (topLevelItem(topLevelItemCount()-1) == item) addItem();
            emit updateWatch(item);
        }
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
    QList<QTreeWidgetItem*> watches;
    for (int i = 0; i < topLevelItemCount(); i++) {
        if (topLevelItem(i)->text(0).isEmpty()) continue;
        watches.append(topLevelItem(i));
    }
    if (!watches.isEmpty())
        emit updateWatches(&watches);
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

void Watcher::expandTable(QTreeWidgetItem *item)
{
    QString text = item->text(2);
    if (item->text(2).compare("table") != 0) return;
    item->setExpanded(true);
    emit updateTable(item);
}

void Watcher::collapseTable(QTreeWidgetItem *table)
{
    table->takeChildren();
}

void Watcher::allUpdated()
{
    // TODO after update set vertical scroll bar as original
}
