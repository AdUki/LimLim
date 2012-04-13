#ifndef WATCHER_H
#define WATCHER_H

#include <QTreeWidget>

class QTreeWidgetItem;

class Watcher : public QTreeWidget
{
Q_OBJECT
public:
    explicit Watcher(QWidget *parent = 0);
    void addItem();

signals:
    void updateWatch(QTreeWidgetItem *watch);
    void updateWatches(QList<QTreeWidgetItem*> *watches);
    void updateTable(QTreeWidgetItem *table);
    void setWatch(QTreeWidgetItem *watch);

public slots:
    virtual void updateAll();
    void allUpdated();

    void addWatch();
    void deleteWatch();

    void clearAllWatches();
    void replaceAllWatches(QList<QTreeWidgetItem*> *watches);

private slots:
    void updateActions();
    void updateItem(QTreeWidgetItem *item, int column);
    void removeSelectedItems();
    void expandTable(QTreeWidgetItem *item);
    void collapseTable(QTreeWidgetItem *table);

private:
    int scrollBarPosition;
};

#endif // WATCHER_H
