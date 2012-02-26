#ifndef WATCHMODEL_H
#define WATCHMODEL_H

#include <QStandardItemModel>

class QTreeView;

class WatchModel : public QStandardItemModel
{
Q_OBJECT
public:
    /** If WatchModel is locked user can't add and remove items
      */
    WatchModel(QTreeView *viewer, QObject *parent = 0, bool locked = false);

signals:
    void updateItem(QStandardItem *item);

public slots:


private slots:
    void itemClicked(QModelIndex index);
    void itemDoubleClicked(QModelIndex index);

private:
    bool locked;
};

#endif // WATCHMODEL_H
