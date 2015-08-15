#include "drmstatus.h"
//#include "ui_drmstatusframe.h"
#include "drm.h"
#include <QDebug>
// #include "qsstvglobal.h"
#include "drmrx/demodulator.h"
#include <math.h>
#include "externs.h"


int currentSegmentNumber;
unsigned int rxTransportID;
emscStatus msc_valid;

QList<short unsigned int> drmBlockList;
int bodyTotalSegments;
int rxSegments;

drmStatusFrame::drmStatusFrame(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::drmStatusFrame)
{
  ui->setupUi(this);
  greenPXM=new QPixmap(40,30);
  greenPXM->fill(Qt::green);
  redPXM=new QPixmap(40,30);
  redPXM->fill(Qt::red);
  yellowPXM=new QPixmap(40,30);
  yellowPXM->fill(Qt::yellow);
  init();
}

drmStatusFrame::~drmStatusFrame()
{
  delete ui;
}

void drmStatusFrame::init()
{
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
  ui->mscLED->setPixmap(*redPXM);
  ui->facLED->setPixmap(*redPXM);
  ui->frameLED->setPixmap(*redPXM);
  ui->timeLED->setPixmap(*redPXM);
}

void drmStatusFrame::paintEvent (QPaintEvent *)
{
  int freqOff;
  ui->modeEdit->setText(mode);
  ui->bandwidthEdit->setText(QString::number(bandwidth));
  ui->interleaveEdit->setText(interleave);
  ui->protectionEdit->setText(protection);
  ui->qamEdit->setText(QString::number(qam));
  ui->snrEdit->setText(QString::number(WMERFAC,'g',2)+" dB");
  freqOff=(int)round(freqOffset-350);
  ui->offsetEdit->setText(QString::number(freqOff)+" Hz");
//  ui->dfsEdit->setText(QString::number(deltaFS,'g',3) + " ppm");
  ui->totalSegmentsEdit->setText(QString::number(totalSeg));
  ui->blocksReceivedFrame->setMaxBlocks(totalSeg);
  ui->rxSegmentsEdit->setText(QString::number(rxSeg));
  ui->callEdit->setText(call);
  ui->transportIDEdit->setText(QString::number(transportID));
  ui->currentSegmentEdit->setText(QString::number(currentSegment));
  ui->blocksReceivedFrame->setBlocks(drmBlockList);
}

void drmStatusFrame::setStatus()
{

  if(demodulatorPtr->isTimeSync())
    {
       ui->timeLED->setPixmap(*greenPXM);
    }
  else
    {
       ui->timeLED->setPixmap(*redPXM);
       ui->facLED->setPixmap(*redPXM);
       ui->frameLED->setPixmap(*redPXM);
       ui->mscLED->setPixmap(*redPXM);
      // update();
       return;
     }
  currentSegment=currentSegmentNumber;
  transportID=rxTransportID;
  totalSeg=bodyTotalSegments;
  rxSeg=rxSegments;
  switch(robustness_mode)
    {
    case 0: mode="A"; break;
    case 1: mode="B"; break;
    case 2: mode="E"; break;
    default: mode=""; break;
    }
  if(callsignValid)
    {
      call=drmCallsign;
     }

  if(fac_valid==1)
    {
       ui->facLED->setPixmap(*greenPXM);
    }
  else
    {
       ui->facLED->setPixmap(*redPXM);
    }

  switch(msc_valid)
    {

      case INVALID:  ui->mscLED->setPixmap(*redPXM); break;
      case VALID: ui->mscLED->setPixmap(*greenPXM); break;
      case ALREADYRECEIVED: ui->mscLED->setPixmap(*yellowPXM); break;
    }
  if(demodulatorPtr->isFrameSync())
    {
       ui->frameLED->setPixmap(*greenPXM);
    }
  else
    {
       ui->frameLED->setPixmap(*redPXM);
    }
  if(demodulatorPtr->isTimeSync())
    {
       ui->timeLED->setPixmap(*greenPXM);
    }
  else
    {
       ui->timeLED->setPixmap(*redPXM);
    }

  if(mode=="") return;
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
 switch(interleaver_depth_new)
   {
   case 0: interleave="Long"; break;
   case 1: interleave="Short"; break;
   default: ; break;
   }

 switch(msc_mode_new)
   {
   case 0: qam=64; break;
   case 1: qam=16; break;
   case 3: qam=4; break;
   default: qam=0; break;
   }
  ui->totalSegmentsEdit->setText(QString::number(bodyTotalSegments));
  ui->rxSegmentsEdit->setText(QString::number(rxSegments));
  update();
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

