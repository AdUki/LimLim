#include "stack.h"
#include <QStringListModel>

Stack::Stack(QWidget *parent) :
    QListView(parent)
{
    model = new QStringListModel(this);
    setModel(model);
    setEditTriggers(QListView::NoEditTriggers);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(functionClicked(QModelIndex)));
}

void Stack::setStack(QStringList *newStack)
{
    model->setStringList(QStringList());
    model->setStringList(*newStack);
}

void Stack::functionClicked(const QModelIndex &index)
{
    QString stackLine = model->stringList().at(index.row());
    QRegExp rx("^(.+):(\\d+): [\\w ']+$");

    rx.indexIn(stackLine);

    QString file = rx.cap(1);
    QString line = rx.cap(2);

    bool ok;
    unsigned num = line.toUInt(&ok);
    if (file.isEmpty() || !ok) return;

    emit highlightSource(file, num);
}
