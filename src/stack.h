#ifndef STACK_H
#define STACK_H

#include <QListView>

class QStringListModel;

class Stack : public QListView
{
Q_OBJECT
public:
    explicit Stack(QWidget *parent = 0);

signals:
    void highlightSource(const QString& file, unsigned line);

public slots:
    void setStack(QStringList *newStack);

private slots:
    void functionClicked(const QModelIndex & index);

private:
    QStringListModel *model;
};

#endif // STACK_H
