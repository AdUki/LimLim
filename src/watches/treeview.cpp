#include "treemodel.h"
#include "treeview.h"

#include <QAction>

const QString InitValue = "";

TreeView::TreeView(TreeModel *model, QWidget *parent)
    : QTreeView(parent)
{
/*
    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    TreeModel *model = new TreeModel(headers, file.readAll());
    file.close();

    for (int column = 0; column < model->columnCount(); ++column)
        this->resizeColumnToContents(column);
*/

    setModel(model);

    connect(this->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(updateActions()));

    actionsMenu = new QAction(tr("Actions menu"), this);
    connect(actionsMenu, SIGNAL(aboutToShow()), this, SLOT(updateActions()));

    insertRowAction = new QAction(tr("New watch"), this);
    connect(insertRowAction, SIGNAL(triggered()), this, SLOT(insertRow()));

    removeRowAction = new QAction(tr("Remove watch"), this);
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));

    insertChildAction = new QAction(tr("Add table field"), this);
    connect(insertChildAction, SIGNAL(triggered()), this, SLOT(insertChild()));

    updateActions();

    addAction(insertRowAction);
    addAction(removeRowAction);
    addAction(insertChildAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void TreeView::insertChild()
{
    QModelIndex index = this->selectionModel()->currentIndex();
    QAbstractItemModel *model = this->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant(InitValue), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant("[No header]"),
                                 Qt::EditRole);
    }

    this->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                            QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool TreeView::insertColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = this->model();
    int column = this->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1, parent);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"),
                             Qt::EditRole);

    updateActions();

    return changed;
}

void TreeView::insertRow()
{
    QModelIndex index = this->rootIndex();

    if (!model()->insertRow(model()->rowCount(), index)) return;

    updateActions();

    for (int column = 0; column < model()->columnCount(index); ++column) {
        QModelIndex child = model()->index(model()->rowCount(), column, index);
        model()->setData(child, QVariant(InitValue), Qt::EditRole);
    }
}

bool TreeView::removeColumn(const QModelIndex &parent)
{
    QAbstractItemModel *model = this->model();
    int column = this->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    bool changed = model->removeColumn(column, parent);

    if (!parent.isValid() && changed)
        updateActions();

    return changed;
}

void TreeView::removeRow()
{
    QModelIndex index = this->selectionModel()->currentIndex();
    QAbstractItemModel *model = this->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void TreeView::updateActions()
{
    bool hasSelection = !this->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);

    bool hasCurrent = this->selectionModel()->currentIndex().isValid();
    //insertRowAction->setEnabled(hasCurrent);

    //if (hasCurrent)
    //    closePersistentEditor(selectionModel()->currentIndex());
}
