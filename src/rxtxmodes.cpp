#include "rxtxmodes.h"
#include <math.h>
#include "drmrx/demodulator.h"
#include "drmrx/structtemplates.h"

extern int spectrum_occupancy_new;
extern int msc_mode_new;
extern int interleaver_depth_new;
extern float freqOffset;
extern float deltaFS;
extern int currentSegmentNumber;
//extern char drmCallsign[9];
//extern demodulator *demodulatorPtr;
extern float WMERFAC;
extern int robustness_mode;
extern int spectrum_occupancy_new;
extern int bodyTotalSegments;
extern struct mplex_desc multiplex_description;
extern int rxSegments;

RxTxModes::RxTxModes( QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
	setupUi(this);
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
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(100);
}

RxTxModes::~RxTxModes()
{
}

void RxTxModes::on_tabWidget_currentChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxModeComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxBandwidthComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxQAMComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxInterleaveComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxProtectionComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxInstancesSpinBox_valueChanged(int)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::on_drmTxReedSolomonComboBox_activated(const QString&)
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
}

void RxTxModes::update()
{
//	qWarning("TODO: %s	%d",__FILE__,__LINE__);
	setStatus();
	freqOff = (int)round(freqOffset);

	offsetEdit->setText(QString::number(freqOff)+" Hz");
	modeEdit->setText(mode);
	bandwidthEdit->setText(QString::number(bandwidth));
//	protectionEdit->setText(protection);
//	qamEdit->setText(QString::number(qam));
	snrEdit->setText(QString::number(WMERFAC,'g',2)+" dB");

}

void RxTxModes::setStatus()
{

  currentSegment = currentSegmentNumber;
  currentSegmentEdit->setText(QString::number(currentSegment));
////  transportID = rxTransportID;
//  totalSeg = bodyTotalSegments;
  rxSeg = rxSegments;
  rxSegmentsEdit->setText(QString::number(rxSegments));
  switch(robustness_mode)
	 {
	 case 0: mode="A"; break;
	 case 1: mode="B"; break;
	 case 2: mode="E"; break;
	 default: mode=""; break;
	 }
//  if(callsignValid)
//	 {
//		call=drmCallsign;
//	  }

//  if(fac_valid==1)
//	 {
//		 ui->facLED->setPixmap(*greenPXM);
//	 }
//  else
//	 {
//		 ui->facLED->setPixmap(*redPXM);
//	 }

//  switch(msc_valid)
//	 {

//		case INVALID:  ui->mscLED->setPixmap(*redPXM); break;
//		case VALID: ui->mscLED->setPixmap(*greenPXM); break;
//		case ALREADYRECEIVED: ui->mscLED->setPixmap(*yellowPXM); break;
//	 }
//  if(demodulatorPtr->isFrameSync())
//	 {
//		 ui->frameLED->setPixmap(*greenPXM);
//	 }
//  else
//	 {
//		 ui->frameLED->setPixmap(*redPXM);
//	 }
//  if(demodulatorPtr->isTimeSync())
//	 {
//		 ui->timeLED->setPixmap(*greenPXM);
//	 }
//  else
//	 {
//		 ui->timeLED->setPixmap(*redPXM);
//	 }

//  if(mode=="") return;
  switch(spectrum_occupancy_new)
  {
	 case 0: bandwidth=2.3; break;
	 case 1: bandwidth=2.5;; break;
	 default:bandwidth=0; break;
  }

 switch (multiplex_description.PL_PartB)
	{
	case 0: protection="High"; break;
	case 1: protection="Low"; break;
	default: ; break;
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
	totalSegmentsEdit->setText(QString::number(bodyTotalSegments));
//  ui->rxSegmentsEdit->setText(QString::number(rxSegments));
//  update();
}
