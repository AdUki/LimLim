#include "variablewatcher.h"
#include <QTableWidgetItem>
#include <QAction>

VariableWatcher::VariableWatcher(const Debugger *debugger)
{
    setColumnCount(3);
    setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name"), QTableWidgetItem::Type));
    setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Type"), QTableWidgetItem::Type));
    setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Value"), QTableWidgetItem::Type));

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *newAction = new QAction(tr("&New watch"), this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new watch"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newWatch()));
    addAction(newAction);

    newAction = new QAction(tr("&Delete watch"), this);
    newAction->setShortcut(QKeySequence("Ctrl+d"));
    newAction->setStatusTip(tr("Deletes selected watches"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(deleteWatch()));
    addAction(newAction);
}

void VariableWatcher::newWatch()
{
    this->setRowCount(this->rowCount() + 1);
}

void VariableWatcher::deleteWatch()
{
    QListIterator<QTableWidgetSelectionRange> iter(selectedRanges());
    iter.toBack();
    while(iter.hasPrevious()) {
        QTableWidgetSelectionRange range = iter.previous();

        for (int n = range.bottomRow(); n >= range.topRow(); n--) {
            this->removeRow(n);
        }
    }
}
