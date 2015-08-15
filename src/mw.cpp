#include "mw.h"

#include <QMessageBox>
#include <QDebug>

#include <iostream>
#include <string>
#include <unistd.h>
#include "version.h"
#include "objnotify.h"
#include "drmtx/bsrform.h"
// #include "drmrx/fixform.h"
// #include "rxwidget.h"
#include "drmsettings.h"
#include "runtx.h"
#include "qpfn.h"

#include "drmtx/drmtransmitter.h"

using namespace std;

///////// externs
// ObjNotify publicNotify;
QVector<_SAMPLE> audio;
sourceDecoder srcDecoder;
DRMReceiver DRMReceiver;
// CDRMTransmitter *DRMTransmitter;
QList<short unsigned int> fixBlockList;
uint txTransportID;
bool stopDRM;
bool toSndFile; // TX To SndFile DRMSignalIO.cpp
int extiBlockCnt;
QPushButton *FixPtr;
float OpVolume = 0.35f;
float specV = 20.0;
int specLen = 1000; // 300 = starting value. Will be set in waterfall widget
LogView *editor;
// TableEditor *sessionSave;

///////// end externs






Mw::Mw( QWidget * parent, Qt::WindowFlags f) : QMainWindow(parent,f)
{
	setupUi(this);
	QString s = "LinDrm Ver: ";
	s += LINDRM_VERSION;
	s += " - DRM for Linux :)";
	setWindowTitle(s);
	nblocks = 0;
	toSndFile = false;
	TX_Rec->setChecked(false);
	DispWf = true;
	FixPtr = Fix;
	editor = new LogView();
	// Set sound
	QString cmd("alsactl --file ~/.config/G4EKQ/LinDrm/asound.state restore");
	system(cmd.toLatin1().data());
	// See also CDRMTransmitter::run()
	stopDRM = true;
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(50);
	DRMReceiver.Start(true);
}

Mw::~Mw()
{
	QString cmd("alsactl --file ~/.config/G4EKQ/LinDrm/asound.state store");
	system(cmd.toLatin1().data());
	editor->saveLogs();
	DRMReceiver.Start(false);
	while(!DRMReceiver.getAck()){usleep(100);}
	qDebug() << __FILE__ << __LINE__ << "DRMReceiver.getAck()" << DRMReceiver.getAck();
}

void Mw::on_tabWidget_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void Mw::on_actionSound_Card_triggered()
{
	Soundcard sc;
	if(sc.exec())
	{
		DRMReceiver.ReReadSoundINdev();
	}
}

void Mw::on_actionSession_Log_triggered()
{
	editor->resize(450,200);
	editor->show();
}

void Mw::on_pic_changed(const QString& s)
{
	imgFname = s;
	img.load(s);
	imgView->setPixmap(img.scaled(imgView->size( ),
											Qt::KeepAspectRatio,
											Qt::SmoothTransformation));

	QString title("Tx Pic PreView: ");
	title += Qpfn::getFname_Ext(imgFname);
	groupBox->setTitle(title);
}

void Mw::resizeEvent(QResizeEvent * /* event */)
{
	if (img.isNull())
		return;
	imgView->setPixmap(img.scaled(imgView->size( ),
											Qt::KeepAspectRatio,
											Qt::SmoothTransformation
											)
							 );

}


void Mw::on_actionDrm_Settings_triggered()
{
	DrmSettings settings;
	if(settings.exec())
	{
		//   settings.readSettings();
		//   settings.setParams();
		settings.GetParams();
		settings.writeSettings();
	}
}


void Mw::update() // test led widget
{
//	static int dbc = 25;
//	if(dbc)
//		dbc--;
//	else
//	{
//		qDebug() << '.';
//		dbc = 25;
//	}

	static bool tog = 0;
	tog ^= 1;
	if(tog)
	{
		widget->update();
		return;
	}

	if(!this->isVisible())
	{
		if(editor->isVisible())
			editor->close();
	}
//	qDebug() << __FILE__ << __LINE__ << editor->isVisible();
	if(!DispWf)
		return;
	if(!DRMReceiver.GetInputSpec(vecrData))
		return;

	WfDisp->updatePlot(vecrData);

	extern bool MSCAvailable;
	extern bool FACAvailable;
	if(MSCAvailable)
		MscFrame->setConstellation(MSC);
	if(FACAvailable)
		FacFrame->setConstellation(FAC);

	extern int bodyTotalSegments;
	extern int rxSegments;
	int missing = bodyTotalSegments - rxSegments;
	if(missing)
	{
		QString m;
		m.setNum(missing);
		Bsr->setText("BSR\n" + m);
	}
	else
		Bsr->setText("BSR");

	if (stopDRM)
	{
		stopDRM = false;
		TxPic->setChecked(false);
		if(toSndFile)
		{
			if (!audio.empty())
			{
				RecFname = imgFname + ".flac";
				SndfileHandle *sfout = new SndfileHandle(
							RecFname.toLatin1().data(), // file name
							SFM_WRITE,
							SF_FORMAT_FLAC | SF_FORMAT_PCM_16, //FORMAT
							1, //CHANNELS
							48000
							);
				sfout->write( &audio[0], audio.size() );
				delete sfout; // close file
				toSndFile = false;
			}
		}
	}
}

void Mw::TextColour(QPushButton *pb, QColor c)
{
	QPalette p;
	QBrush b(c);
	b.setStyle(Qt::SolidPattern);
	p.setBrush(QPalette::Inactive, QPalette::ButtonText, b);
	p.setBrush(QPalette::Active, QPalette::ButtonText, b);
	pb->setPalette(p);
}

void Mw::fromPlayList()
{

}

void Mw::on_FromFile_toggled(bool x)
{
	if (x)
	{
		QFile playList("/home/castro/Work-64/LinDrm-qt5-TX/play.lst");

		if (!playList.open(QIODevice::ReadOnly | QIODevice::Text))
			 return;

		while (!playList.atEnd())
		{
			QString fn;
			 fn = playList.readLine().simplified();
//			 qDebug() << __FILE__ << __LINE__ << fn;
			 DRMReceiver.openSoundFile(fn,false);
		}


	  TextColour(FromFile, Qt::red);

	}
	else
	{
		TextColour(FromFile, Qt::black);
		DRMReceiver.openSoundFile("",false);
	}

}

void Mw::on_Record_toggled(bool x)
{
	qWarning("Record: %d, TODO: %s	%d", x, __FILE__, __LINE__);
	QString fileName;
	//	soundINPtr->setRec(x);

	if (x)
	{
		audio.clear();
		TextColour(Record, Qt::red);
		Record->setText("Recording");
		RecFname.clear();
	}
	else
	{
		TextColour(Record, Qt::black);
		Record->setText("Record");
		if (!audio.empty())
		{
			if(RecFname.isEmpty ())
			{
				RecFname = "Record.flac";
			}
			else
			{
				RecFname += ".flac";
			}
			SndfileHandle *sfout = new SndfileHandle(
						RecFname.toLatin1().data(), // file name
						SFM_WRITE,
						SF_FORMAT_FLAC | SF_FORMAT_PCM_16, //FORMAT
						1, //CHANNELS
						48000
						);
			sfout->write( &audio[0], audio.size() );
			delete sfout; // close file
		}
	}
}

void Mw::on_TxPic_clicked()
{
	if(imgFname.isEmpty()) return;
	editor->insert("ME");

	DRMReceiver.Start(false);
	WfDisp->clear();
	WfDisp->update();
	DispWf = false;

	txProgress *pg = new txProgress(IMAGE);
	pg->setImgFname(imgFname);
	pg->exec();
	DRMReceiver.Start(true);
	DispWf = true;
}



void Mw::on_Bsr_clicked()
{
	qDebug() << __FILE__ << __LINE__;

	DRMReceiver.Start(false);
	WfDisp->clear();
	WfDisp->update();
	DispWf = false;
	txProgress *pg = new txProgress(BSR);
	bsrForm *bsrf = new bsrForm;
	switch(bsrf->exec())
	{
	case CANCEL:
		qDebug() << __FILE__ << __LINE__ << "CANCEL";
		return;
	case EASYPAL:
		qDebug() << __FILE__ << __LINE__ << "EASYPAL";
		break;
	case COMPAT:
		qDebug() << __FILE__ << __LINE__ << "COMPAT";
		break;
	}
	QByteArray *ba;
	ba = bsrf->getBA(COMPAT);
	drmTxParams drmParams;
	drmParams = bsrf->getDRMParams();
	pg->setBSR(ba,drmParams);
	pg->exec();
	DRMReceiver.Start(true);
	DispWf = true;
}

void Mw::on_Fix_clicked()
{
	qDebug() << __FILE__ << __LINE__;
	DRMReceiver.Start(false);
	WfDisp->clear();
	WfDisp->update();
	DispWf = false;
	txProgress *pg = new txProgress(FIX);
	extern txSession FixSession;
	pg->setFIX(&FixSession);

	pg->exec();
	DRMReceiver.Start(true);
	DispWf = true;
}

void Mw::onVolumeChanged(int v)
{
	OpVolume = v / 100.0f;
//	qDebug() << __FILE__ << __LINE__ << OpVolume;
}

void Mw::onSpecChanged(int v)
{
	specV = v;
//	qDebug() << __FILE__ << __LINE__ << OpVolume;
}


