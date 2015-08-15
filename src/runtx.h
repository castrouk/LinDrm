#ifndef RUNTX_H
#define RUNTX_H

#include <QThread>
#include <QFileInfo>
#include <QString>
#include <QMessageBox>

#include "dsp/filterparam.h"
#include "drmtx/drmtransmitter.h"
#include "utils/reedsolomoncoder.h"
#include "drmsettings.h"

struct txSession
{
	drmTxParams drmParams;
	QByteArray ba;  // contains the image data in jpg, jp2 .... format
	uint transportID;
	QString filename;
	QString extension;
};

class RunTx
{
public:
	RunTx();
	~RunTx();
	void initDRMImage(QString);
	void initDRMBSR(QByteArray *ba);
	bool initDRMFIX(QString fileName, QString extension, eRSType rsType, int mode);
	bool initDRMFIX(txSession *sessionPtr);
	void setDRMTxParams(drmTxParams params)
	{
	  drmTxParameters=params;
	}
	//    void setPicFname(QString f){ picfilename = f; }
	void sendDRMFIX();
	int getBlocks() {return txDRM->getBlocks();}
	double getDuration() {return txDRM->transmissionTime;}
	txSession *getSessionPtr(uint transportID);
	bool prepareFIX(QByteArray bsrByteArray);

private:
//	QList <txSession> txList;
	QString m_picfilename;
	drmTxParams drmTxParameters;
	QByteArray baDRM;
	drmTransmitter *txDRM;
};

#endif // RUNTX_H
