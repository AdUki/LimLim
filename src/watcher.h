#ifndef WATCHER_H
#define WATCHER_H

#include <QTreeView>

class QStandardItemModel;
class QStandardItem;

class Watcher : public QTreeView
{
Q_OBJECT
public:
    explicit Watcher(QWidget *parent = 0);

signals:
    void updateWatch(QStandardItem *watch);

public slots:
    void updateAll();

private slots:
    void updateActions(const QModelIndex &index);
    void updateItem(QStandardItem *item);

private:
    QStandardItemModel* model;
};

#endif // WATCHER_H
