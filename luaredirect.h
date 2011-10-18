#ifndef LUAREDIRECT_H
#define LUAREDIRECT_H

#include <QObject>
#include <QSocketNotifier>

class luaredirect : public QObject
{
    Q_OBJECT

public:
    explicit luaredirect(QObject *parent = 0);

	//bool setProcess();

signals:
	void outputReady(QString output);

public slots:
	//void writeInput(QString input);

};

#endif // LUAREDIRECT_H
