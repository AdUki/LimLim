#include "watchmodel.h"

#include <QTreeView>

WatchModel::WatchModel(QTreeView *viewer, QObject *parent)
    : QStandardItemModel(0, 3, parent)
{
    setHeaderData(0, Qt::Horizontal, QVariant(tr("Expression")), Qt::DisplayRole);
    setHeaderData(1, Qt::Horizontal, QVariant(tr("Value")), Qt::DisplayRole);
    setHeaderData(2, Qt::Horizontal, QVariant(tr("Type")), Qt::DisplayRole);
}
