#ifndef TXPROGRESS_H
#define TXPROGRESS_H

#include "ui_txProgress.h"
#include <QWidget>
#include <QTimer>
#include <QDebug>
#include "runtx.h"
#include <QDialog>

// class txProgress : public QWidget , private Ui::txProgress

enum txMode {IMAGE,BSR,FIX};

class txProgress : public QDialog , private Ui::txProgress
{
	Q_OBJECT
public:
	txProgress(int mode);
	~txProgress();
	void setImgFname(QString s);
	void setFIX(txSession *);
	void setBSR(QByteArray *,drmTxParams);
	void resetAll()
	{
		TxDone = false;
		WfDone = false;
	}


private slots:
	void upDateWF(int v);
	void upDateTX(int v);
	void StopTx();
	void on_SendGlitch_clicked();

private:
	int count;
	int m_max;
	bool autoClose;
	QTimer *timer;
	RunTx *TX;
	QString imgFname;
	int nBlocks;
	int WfLen;
	bool TxDone = false;
	bool WfDone = false;
	int m_txmode;
	QString m_home;
};
#endif //TXPROGRESS_H















