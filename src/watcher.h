#ifndef WATCHER_H
#define WATCHER_H

#include <QTreeView>

class QStandardItemModel;

class Watcher : public QTreeView
{
Q_OBJECT
public:
    explicit Watcher(QWidget *parent = 0);

private slots:
    void updateActions(const QModelIndex &index);

private:
    QStandardItemModel* model;
};

#endif // WATCHER_H
