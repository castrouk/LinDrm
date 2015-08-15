#ifndef OBJNOTIFY_H
#define OBJNOTIFY_H

#include <QObject>
#include <QDebug>

class ObjNotify : public QObject
{
	Q_OBJECT
public:
	explicit ObjNotify(QObject *parent = 0);
	~ObjNotify();
	void ClearTx(){TxCount = 0;}
	void incTxCount(){emit TxCountChanged(TxCount++);}

signals:
	void TxCountChanged(int TxCount);

public slots:

private:
int TxCount = 0;
};

#endif // OBJNOTIFY_H
