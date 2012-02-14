#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class TreeModel;

class TreeView : public QTreeView
{
Q_OBJECT
public:
    explicit TreeView(TreeModel *model, QWidget *parent = 0);

public slots:
    // Change actions according to items in tree
    void updateActions();

private slots:
    void insertChild();
    bool insertColumn(const QModelIndex &parent = QModelIndex());
    void insertRow();
    bool removeColumn(const QModelIndex &parent = QModelIndex());
    void removeRow();

private:
    QAction *actionsMenu;
    QAction *insertRowAction;
    QAction *removeRowAction;
    QAction *insertChildAction;
};

#endif // TREEVIEW_H
