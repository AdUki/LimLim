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
    //connect(actionsMenu, SIGNAL(aboutToShow()), this, SLOT(updateActions()));

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

    QString text = model()->data(currentIndex()
                                 .sibling(currentIndex().row(), 2))
            .toString();
    // TODO get text "table" via global constant
    insertChildAction->setEnabled(text.compare("table", Qt::CaseInsensitive) == 0);

    //if (hasCurrent)
    //    closePersistentEditor(selectionModel()->currentIndex());
}
