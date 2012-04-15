#ifndef HIDEEVENTWATCHER_H
#define HIDEEVENTWATCHER_H

#include <QObject>

class HideEventWatcher : public QObject
{
Q_OBJECT
public:
    HideEventWatcher(QObject *parent = 0);

signals:
    void isHidden(bool b);
    void isShown(bool b);
    void hidden();
    void shown();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // HIDEEVENTWATCHER_H
