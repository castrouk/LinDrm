#include "runtx.h"
#include <unistd.h>
#include <QDebug>
#include "sound/sound.h"
#include "soundcard.h"

extern QList<short unsigned int> fixBlockList;
extern QList <txSession> txList;

int opCount;

RunTx::RunTx()
{
	qDebug() << __FILE__ << __LINE__;
	txDRM = new drmTransmitter;
	qDebug() << __FILE__ << __LINE__;
	txList.clear();
	DrmSettings settings;
	settings.GetParams();

}

RunTx::~RunTx()
{
	//    sleep(1);
	txDRM->start(false);

	txDRM->~drmTransmitter();
}


//struct txSession
//{
//  drmTxParams drmParams;
//  QByteArray ba;  // contains the image data in jpg, jp2 .... format
//  uint transportID;
//  QString filename;
//  QString extension;
//};



void RunTx::initDRMImage(QString picfilename)
{
	qDebug() << __FILE__ << __LINE__ << picfilename;
	eRSType rsType;
	reedSolomonCoder rsd;
//	m_picfilename = picfilename;

	QFileInfo finf(picfilename);
	fixBlockList.clear();

	DrmSettings s;
	drmTxParameters = s.GetDrmParams();

	if(txList.count()>5) txList.removeFirst();
	txList.append(txSession());
	txList.last().drmParams = drmTxParameters;
	txList.last().filename = finf.baseName();
	txList.last().extension = finf.suffix();
	QFile file(picfilename);
//	qDebug() << __FILE__ << __LINE__ << picfilename;
	if (!file.open(QIODevice::ReadOnly)) return;
	txList.last().ba = file.readAll(); // writes image into ba in * format
//	qDebug() << __FILE__ << __LINE__;
	rsType=(eRSType)txList.last().drmParams.readSolomon;
	baDRM=txList.last().ba;
//	qDebug() << __FILE__ << __LINE__;

	if(rsType!=RSTNONE)
	{
		qDebug() << __FILE__ << __LINE__;
		rsd.encode(baDRM,txList.last().extension,rsType);
		txDRM->init(&baDRM,txList.last().filename,rsTypeStr[rsType],txList.last().drmParams);
	}
	else
	{
		qDebug() << __FILE__ << __LINE__;
		txDRM->init(&baDRM,txList.last().filename,txList.last().extension,txList.last().drmParams);
		qDebug() << __FILE__ << __LINE__;
	}
	// transportID is set

	qDebug() << __FILE__ << __LINE__ << "Do i need txTransportID";

	txList.last().transportID = txTransportID;

	txDRM->start(true);
}


void RunTx::initDRMBSR(QByteArray *ba)
{
	baDRM=*ba;
	fixBlockList.clear();
	txDRM->init(&baDRM,"bsr","bin",drmTxParameters);
	///  addToLog(QString("bsr.bin send").arg(baDRM.size()),LOGPERFORM);
	txDRM->start(true);
}

bool RunTx::initDRMFIX(QString fileName,QString extension,eRSType rsType,int mode)
{
	reedSolomonCoder rsd;
	QFile fi(fileName);
	if(fi.open(QIODevice::ReadOnly)<=0) return FALSE;
	baDRM=fi.readAll();
	if(rsType!=RSTNONE)
	{
		rsd.encode(baDRM,extension,rsType);
		txDRM->init(&baDRM,fileName,rsTypeStr[rsType],modeToParams(mode));
	}
	else
	{
		txDRM->init(&baDRM,fileName,extension,modeToParams(mode));
	}
	return TRUE;
}

bool RunTx::initDRMFIX(txSession *sessionPtr)
{
	qDebug() << __FILE__ << __LINE__ << "sessionPtr->filename" << sessionPtr->filename;
	reedSolomonCoder rsd;
	eRSType rsType;
	rsType=(eRSType)sessionPtr->drmParams.readSolomon;
	baDRM=sessionPtr->ba;
	if(rsType!=RSTNONE)
	{
		qDebug() << __FILE__ << __LINE__;
		rsd.encode(baDRM,sessionPtr->extension,rsType);
		txDRM->init(&baDRM,sessionPtr->filename,rsTypeStr[rsType],sessionPtr->drmParams);
	}
	else
	{
		qDebug() << __FILE__ << __LINE__;
		txDRM->init(&baDRM,sessionPtr->filename,sessionPtr->extension,sessionPtr->drmParams);
	}
	qDebug() << __FILE__ << __LINE__;
	txDRM->start(true);
	return TRUE;
}


void RunTx::sendDRMFIX()
{
  QByteArray ba;
  QString rxImagesPath = "/home/castro/Work-64/LinDrm/RxPicNew";
  qDebug() << __FILE__ << __LINE__ << "To Do QFile fi(rxImagesPath)";
  QFile fi(rxImagesPath+"/bsr.bin");
  if(fi.open(QIODevice::ReadOnly)<=0) return;
  ba=fi.readAll();
  txDRM->init(&ba,"bsr","bin",drmTxParameters);
  txDRM->start(true);
}

