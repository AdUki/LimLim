#ifndef WATCHMODEL_H
#define WATCHMODEL_H

#include <QStandardItemModel>

class QTreeView;

class WatchModel : public QStandardItemModel
{
Q_OBJECT
public:
    WatchModel(QTreeView *viewer, QObject *parent = 0);
};

#endif // WATCHMODEL_H
