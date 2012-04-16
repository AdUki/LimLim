#include "hideeventwatcher.h"
#include <QEvent>

HideEventWatcher::HideEventWatcher(QObject *parent)
    : QObject(parent)
{
}

bool HideEventWatcher::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::Hide) {
        emit isHidden(true);
        emit isShown(false);
        emit hidden();
    } else if (event->type() == QEvent::Show) {
        emit isHidden(false);
        emit isShown(true);
        emit shown();
    }
    event->accept();
    return true;
}
