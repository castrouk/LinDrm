#include "rxstatus.h"
//#include "ui_RxStatus.h"
#include <math.h>
#include "drmrx/demodulator.h"
#include "drmrx/structtemplates.h"
#include "drmrx/sourcedecoder.h"
// #include "widgets/blockview.h"
#include "logs/logview.h"

QColor Io_Led(Qt::red);
QColor Freq_Led(Qt::red);
QColor Time_Led(Qt::red);
QColor Frame_Led(Qt::red);
QColor FAC_Led(Qt::red);
QColor MSC_Led(Qt::red);
//enum emscStatus {INVALID,VALID,ALREADYRECEIVED};

enum emscStatus {INVALID,VALID,ALREADYRECEIVED};
bool newSamp;

RxStatus::RxStatus( QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
	setupUi(this);
	//	QColor Io_Led = Qt::red;
	//	Freq_Led = Qt::red;
	//	Time_Led = Qt::red;
	//	Frame_Led = Qt::red;
	//	FAC_Led = Qt::red;
	//	MSC_Led = Qt::red;
	mode="";
	bandwidth=0;
	interleave="";
	protection="";
	qam=0;
	call="";
	currentSegment=0;
	transportID=0;
	totalSeg=0;
	rxSeg=0;
	newSamp = false;
//	QTimer *timer = new QTimer(this);
//	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
//	timer->start(70);
}

RxStatus::~RxStatus()
{
}

void RxStatus::SetLed(QLabel *Led, QColor c)
{
	QPalette p;
	p.setColor(QPalette::Background, c);
	Led->setPalette(p);
}

#define MAXLIST 2

int RxStatus::chkList(QStringList &list)
{
	int res = 0;
	for(int i = 0; i < list.size() - 1; i++)
	{
		if(list.at(i).contains(list.at(i + 1)))
			res++;
	}
	if(list.size() > MAXLIST)
	{
		list.pop_front();
	}
	return res;
}


void RxStatus::update()
{
//	qDebug() << __FILE__ << __LINE__;
	//	if(!newSamp)
	//		return;
	extern demodulator *demodulatorPtr;
	extern int currentSegmentNumber;
	extern int bodyTotalSegments;
	extern int rxSegments;
	extern int robustness_mode;
	extern bool callsignValid;
	extern LogView *editor;

	extern char drmCallsign[9];
	extern int fac_valid;
	extern int spectrum_occupancy_new;
	extern struct mplex_desc multiplex_description;
	extern int interleaver_depth_new;
	extern int msc_mode_new;
	extern emscStatus msc_valid;
	extern unsigned int rxTransportID;
	extern QString rxFname;
	extern float WMERFAC;
	extern float freqOffset;
	extern QList<short unsigned int> drmBlockList;
	SetLed(Led_Io, Io_Led);
	SetLed(Led_Freq, Freq_Led);
	SetLed(Led_Time, Time_Led);
	SetLed(Led_Frame, Frame_Led);
	SetLed(Led_FAC, FAC_Led);
	SetLed(Led_MSC, MSC_Led);

//	fnamelist << rxFname;
//	if(chkList(fnamelist) > 1)
//	{
		if(!RxFname->text().contains(rxFname))
			RxFname->setText(rxFname);
//	}

	snrEdit->setText(QString::number(WMERFAC,'g',2)+" dB");
	freqOff = (int)round(freqOffset);
	offsetEdit->setText(QString::number(freqOff)+" Hz");

	if(demodulatorPtr->isTimeSync())
	{
		//	  qDebug() << __FILE__ << __LINE__;
		Time_Led = Qt::green;
	}
	else
	{
		//	  qDebug() << __FILE__ << __LINE__;
		//		 Freq_Led = Qt::red;
		Time_Led = Qt::red;
		Frame_Led = Qt::red;
		FAC_Led = Qt::red;
		MSC_Led = Qt::red;
		return;
	}
	txportEdit->setText(QString::number(rxTransportID));
	currentSegment=currentSegmentNumber;
	currentSegmentEdit->setText(QString::number(currentSegment));
	totalSeg=bodyTotalSegments;
	totalSegmentsEdit->setText(QString::number(totalSeg));
	rxSeg=rxSegments;
	switch(robustness_mode)
	{
	case 0: mode="A"; break;
	case 1: mode="B"; break;
	case 2: mode="E"; break;
	default: mode=""; break;
	}
	modeEdit->setText(mode);

	if(callsignValid)
	{
		call = drmCallsign;
		callsignValid = false;
		calllist << call;
		if(chkList(calllist) > MAXLIST - 1)
		{
			if(!CallEdit->text().contains(call))
			{
				CallEdit->setText(call);
				editor->insert(call);
			}
		}
	}

	if(fac_valid==1)
	{
		FAC_Led = Qt::green;
	}
	else
	{
		FAC_Led = Qt::red;
	}
//	static QString ncall;
	switch(msc_valid)
	{
	case INVALID:  MSC_Led = Qt::red; break;
	case VALID:
		MSC_Led = Qt::green;
//		if(!ncall.contains(call))
//		{
//			editor->insert(call);
//			ncall = call;
//		}
		break;
	case ALREADYRECEIVED: MSC_Led = Qt::yellow ; break;
	}
	if(demodulatorPtr->isFrameSync())
	{
		Frame_Led = Qt::green;
		//		 ui->frameLED->setPixmap(*greenPXM);
	}
	else
	{
		Frame_Led = Qt::red;
		//		 ui->frameLED->setPixmap(*redPXM);
	}
	if(demodulatorPtr->isTimeSync())
	{
		Time_Led = Qt::green;
		//		 ui->timeLED->setPixmap(*greenPXM);
	}
	else
	{
		Time_Led = Qt::red;
		//		 ui->timeLED->setPixmap(*redPXM);
	}

	if(mode=="") return;
	switch(spectrum_occupancy_new)
	{
	case 0: bandwidth=2.3; break;
	case 1: bandwidth=2.5;; break;
	default:bandwidth=0; break;
	}
	bandwidthEdit->setText(QString::number(bandwidth));

	switch (multiplex_description.PL_PartB)
	{
	case 0: protection="High"; break;
	case 1: protection="Low"; break;
	default: break;
	}
	protectionEdit->setText(protection);

	switch(interleaver_depth_new)
	{
	case 0: interleave="Long"; break;
	case 1: interleave="Short"; break;
	default: ; break;
	}
	interleaveEdit->setText(interleave);

	switch(msc_mode_new)
	{
	case 0: qam=64; break;
	case 1: qam=16; break;
	case 3: qam=4; break;
	default: qam=0; break;
	}
	qamEdit->setText(QString::number(qam));
	rxSegmentsEdit->setText(QString::number(rxSegments));
	blocksReceivedFrame->setMaxBlocks(totalSeg);
	blocksReceivedFrame->setBlocks(drmBlockList);
	//	qDebug() << __FILE__ << __LINE__ << totalSeg;
	newSamp = false;
}


QString modeToString(uint mode)
{
	QString tmp;
	tmp+="Mode: ";
	switch(mode/10000)
	{
	case 0: tmp+="A"; break;
	case 1: tmp+="B"; break;
	case 2: tmp+="E"; break;
	default: tmp+="-"; break;
	}
	tmp+="\nBW: ";
	mode-=(mode/10000)*10000;
	switch(mode/1000)
	{
	case 0: tmp+="2.3"; break;
	case 1: tmp+="2.5";; break;
	default:tmp+="---"; break;
	}
	tmp+="\nProt: ";
	mode-=(mode/1000)*1000;
	switch(mode/100)
	{
	case 0: tmp+="High"; break;
	case 1: tmp+="Low"; break;
	default:tmp+="---" ; break;
	}

	tmp+="\nQAM: ";
	mode-=(mode/100)*100;
	switch(mode/10)
	{
	case 0: tmp+="4"; break;
	case 1: tmp+="16"; break;
	case 2: tmp+="64"; break;
	default: tmp+="--"; break;
	}
	return tmp;
}

