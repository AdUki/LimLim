#ifndef WATCHER_H
#define WATCHER_H

#include <QTreeWidget>

class QTreeWidgetItem;

class Watcher : public QTreeWidget
{
Q_OBJECT
public:
    explicit Watcher(QWidget *parent = 0);

signals:
    void updateWatch(QTreeWidgetItem *watch);

public slots:
    void updateAll();

    void addWatch();
    void deleteWatch();
    void clearAllWatches();

private slots:
    void updateActions();
    void updateItem(QTreeWidgetItem *item, int column);
    void addItem();
    void removeSelectedItems();
};

#endif // WATCHER_H
