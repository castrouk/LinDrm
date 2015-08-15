#include "txprogress.h"
#include "objnotify.h"
#include "qpfn.h"

CDRMTransmitter *DRMTransmitter;
ObjNotify *publicNotify;

txProgress::txProgress(int mode) : m_txmode(mode)
{
	setupUi(this);

	QString m;
	switch(m_txmode)
	{
	case IMAGE:
		m = "(IMAGE)"; break;
	case BSR:
		m = "(BSR)"; break;
	case FIX:
	default:
		m = "(FIX)"; break;
	}

	setWindowTitle("Tx Progress " + m);
	publicNotify = new ObjNotify;
	DRMTransmitter = new CDRMTransmitter;
	connect(DRMTransmitter, SIGNAL(upDateWF(int )), this, SLOT(upDateWF(int )));
	connect(publicNotify, SIGNAL(TxCountChanged(int)), this, SLOT(upDateTX(int )));
	connect(DRMTransmitter, SIGNAL(EndTx()), this, SLOT(StopTx()));
	m_home = getenv("HOME");

QString wf;
	switch(m_txmode)
	{
	case IMAGE:
		m = "(IMAGE)";
		wf = "/.config/G4EKQ/default.flac"; break;
	case BSR:
		m = "(BSR)";
		wf = "/.config/G4EKQ/bsr.xbm.flac"; break;
	case FIX:
	default:
		m = "(FIX)";
		wf = "/.config/G4EKQ/fix.xbm.flac"; break;
	}
	DRMTransmitter->setWFfile(m_home + wf);
	QFileInfo inf( m_home + wf);
	WfLen = inf.size();
	label->setText("Sending Wf Text");
}

txProgress::txProgress::~txProgress()
{

}

void txProgress::setImgFname(QString s)
{
	TX = new RunTx;
	imgFname = s;
	TX->initDRMImage(imgFname);
	nBlocks = TX->getBlocks() * 1.16194331983805668f;
}

void txProgress::setBSR(QByteArray *ba, drmTxParams tp)
{
	TX = new RunTx;
	TX->setDRMTxParams(tp);
	TX->initDRMBSR(ba);

}

void txProgress::setFIX(txSession *Session)
{
	TX = new RunTx;
//	extern txSession *FixSessionPtr;

//	TX->setDRMTxParams(tp);
	TX->initDRMFIX(Session);

}


void txProgress::StopTx()
{
	extern bool stopDRM;
	stopDRM = true;
	DRMTransmitter->Stop();
	close();
}

void txProgress::upDateWF(int v)
{
	if(!WfDone)
	{
		label->setText("Sending Wf Text");
		progressBar->setRange(0,WfLen / 24750.59375f);
		WfDone = true;
	}
	progressBar->setValue(v);
}

void txProgress::upDateTX(int v)
{
	if(!TxDone)
	{
		label->setText("Sending: " + Qpfn::getFname_Ext(imgFname));
		progressBar->setRange(0,nBlocks);
		TxDone = true;
	}
//	qDebug() << __FILE__ << __LINE__ << nBlocks << v;
	progressBar->setValue(v);
}

int Glitch = 0;
void txProgress::on_SendGlitch_clicked()
{
	qDebug() << __FILE__ << __LINE__;
	Glitch = 10000;
}
