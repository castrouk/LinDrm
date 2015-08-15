/***************************************************************************
 *   Copyright (C) 2000-2008 by Johan Maes                                 *
 *   on4qz@telenet.be                                                      *
 *   http://users.telenet.be/on4qz                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "txfunctions.h"
#include "qsstvglobal.h"
#include "configparams.h"
#include <qmutex.h>
//#include "imagecoder.h"
#include "dsp/synthes.h"
#include "sstv/modes/modes.h"
#include "dispatcher.h"
#include "sstv/cw.h"
#include <QApplication>
#include <assert.h>
#include "txwidget.h"
#include "sound/soundio.h"
#include "sound/waterfalltext.h"


#define SILENCEDELAY 0.600           // send silence after transmission


QMutex txMutex;

esstvMode modeIndexTx;
int templateIndex;
bool useTemplate;
bool useCW;
bool useVOX;

txFunctions::txFunctions()
{
  synthesPtr=new synthesizer(txClock);
  currentMode=0;
  oldMode=NOTVALID;
  txState=TXIDLE;
  started=FALSE;
  txDRM=new drmTransmitter;
  txList.clear();
}

txFunctions::~txFunctions()
{
  delete synthesPtr;
  if(currentMode) delete currentMode;
}

void txFunctions::run()
{
  abort=FALSE;
  init();
  while(!abort)
    {
      progressTXEvent *ce;
      started=TRUE;
      switch (txState)
        {
        case TXIDLE:
          msleep(10);
        break;
        case TXACTIVE:
          msleep(1);
        break;
        case TXSENDTONE:
          addToLog("txFunc: entered TXSENDTONE",LOGTXFUNC);
          synthesPtr->sendTone(toneDuration,toneLowerFrequency,toneUpperFrequency,FALSE);
          addToLog("txFunc: TXSENDTONE waiting for end",LOGTXFUNC);
          waitEnd();
          txState=TXIDLE;
        break;
        case TXSENDDRM:
//          synthesPtr->sendTone(5,1900,1900,FALSE);
          waterfallTime=waterfallPtr->getDuration(endPicWF);
          waterfallTime+=waterfallPtr->getDuration(startPicWF);
          initDRMImage();
          ce=new progressTXEvent(calcTxTime(TRUE,waterfallTime));
          QApplication::postEvent( dispatcherPtr, ce );  // Qt will delete it when done
          addToLog("start of wf",LOGTXFUNC);
          synthesPtr->sendWFText();
          addToLog("start of txDrm",LOGTXFUNC);
          txDRM->start(TRUE);

          addToLog("end of txDrm",LOGTXFUNC);
          if(txState==TXSENDDRM) // abort if txState is idle
            {
              waterfallPtr->setText(endPicWF);
              synthesPtr->sendWFText();
              addToLog("end of wf",LOGTXFUNC);
              addToLog("txFunc: TXSENDDRM waiting for end",LOGTXFUNC);
              waitEnd();
            }
          txState=TXIDLE;
        break;
        case TXSENDDRMBSR:
          waterfallTime=waterfallPtr->getDuration(bsrWF);
          ce=new progressTXEvent(calcTxTime(TRUE,waterfallTime));
          QApplication::postEvent( dispatcherPtr, ce );  // Qt will delete it when done
          synthesPtr->sendWFText();
          txDRM->start(TRUE);
          addToLog("txFunc: TXSENDDRMBSR waiting for end",LOGTXFUNC);
          waitEnd();
          txState=TXIDLE;
        break;

        case TXSENDDRMFIX:
          waterfallTime=waterfallPtr->getDuration(fixWF);
          ce=new progressTXEvent(calcTxTime(TRUE,waterfallTime));
          QApplication::postEvent( dispatcherPtr, ce );  // Qt will delete it when done
          synthesPtr->sendWFText();
          txDRM->start(TRUE);
          addToLog("txFunc: TXSENDDRMFIX waiting for end",LOGTXFUNC);
          if(txState==TXSENDDRMFIX) // abort if txState is idle
            {
              waterfallPtr->setText(endPicWF);
              synthesPtr->sendWFText();
              addToLog("end of wf",LOGTXFUNC);
              addToLog("txFunc: TXSENDDRM waiting for end",LOGTXFUNC);
              waitEnd();
            }
          txState=TXIDLE;
        break;

        case TXSENDID:
          addToLog("txFunc: entered TXSENDID",LOGTXFUNC);
          ce=new progressTXEvent(waterfallPtr->getDuration(NULL));
          QApplication::postEvent( dispatcherPtr, ce );  // Qt will delete it when done
          synthesPtr->sendWFText();
          addToLog("txFunc: TXSENDID waiting for end",LOGTXFUNC);
          waitEnd();
          txState=TXIDLE;
        break;

        case TXFAXSTART:
          addToLog("txFunc: TXFAXSTART",LOGTXFUNC);
          //            initSSTVImage();
          faxStart();
          addToLog("txFunc: TXFAXSTART waiting for end",LOGTXFUNC);
          waitEnd();
          txState=TXIDLE;
        break;

        case TXSSTVIMAGE:
          addToLog("txFunc: entered TXIMAGE",LOGTXFUNC);
          //            initSSTVImage();
          ce=new progressTXEvent(calcTxTime(FALSE,0));
          QApplication::postEvent( dispatcherPtr, ce );  // Qt will delete it when done
          if(txSSTVParam.mode==FAX480)
            {
              for (int i=0;i<1220;i++)
                {
                  synthesPtr->sendTone(0.00205,1500,0,TRUE);
                  synthesPtr->sendTone(0.00205,2300,0,TRUE);
                }
            }
          else
            {
              sendPreamble();
              sendVIS();
            }
          addToLog("txFunc: sendImage",LOGTXFUNC);
          sendImage();
          addToLog("txFunc: endImage",LOGTXFUNC);
          txState=TXSSTVPOST;
        break;
        case TXSSTVPOST:
          addToLog("txFunc: TXSSTVPOST ",LOGTXFUNC);
          sendCW();
          waitEnd();
          txState=TXIDLE;
        break;
        case TXTEST:
          sendTestPattern();
        break;

        }
    }
  started=FALSE;
  addToLog("txFunc stopped",LOGTXFUNC);
  abort=FALSE;
}



void txFunctions::init()
{
  txState=TXIDLE;
  addToLog("txFunc: Init",LOGTXFUNC);
	sampleCounter=0;
}

void txFunctions::stopAndWait()
{

  stopDRM=TRUE;
  if(!isRunning()) return;
  if(currentMode) currentMode->abort();
  addToLog("txFunc: stop initiated",LOGTXFUNC);
  logStatus();
  txState=TXIDLE;
  while(txState!=TXIDLE)
    {
      if(!isRunning()) return;
      qApp->processEvents();
    }
  addToLog("txFunc: stop executed",LOGTXFUNC);
  stopTXEvent *ce = new  stopTXEvent();
  QApplication::postEvent(dispatcherPtr, ce);
}




bool txFunctions::create(esstvMode m,DSPFLOAT clock)
{
  if(oldMode==m)
    {
      currentMode->init(clock);
      return TRUE;
    }
  oldMode=m;
  if(currentMode) delete currentMode;
  currentMode=0;
  switch (m)
    {
      case M1:
      case M2:
        currentMode=new modeGBR(m,TXSTRIPE,TRUE);
      break;
      case S1:
      case S2:
      case SDX:
        currentMode=new modeGBR2(m,TXSTRIPE,TRUE);
      break;
      case R36:
        currentMode=new modeRobot1(m,TXSTRIPE,TRUE);
      break;
      case R24:
      case R72:
          currentMode=new modeRobot2(m,TXSTRIPE,TRUE);
    break;
      case SC2_60:
      case SC2_120:
      case SC2_180:
      case P3:
      case P5:
      case P7:
          currentMode=new modeRGB(m,TXSTRIPE,TRUE);
      break;
      case FAX480:
      case BW8:
      case BW12:
          currentMode=new modeBW(m,TXSTRIPE,TRUE);
      break;
      case AVT24:
      case AVT90:
      case AVT94:
          currentMode=new modeAVT(m,TXSTRIPE,TRUE);
      break;
      case PD50:
      case PD90:
      case PD120:
      case PD160:
      case PD180:
      case PD240:
      case PD290:
      case MP73:
      case MP115:
      case MP140:
      case MP175:
          currentMode=new modePD(m,TXSTRIPE,TRUE);
      break;
      default:
        m=NOTVALID;
      break;
    }
  if (m!=NOTVALID)
    {
      initializeSSTVParametersIndex(m,TRUE);
      QString s=getSSTVModeNameLong(m);
      addToLog("create: create TX mode",LOGTXFUNC);
      assert(currentMode);
     currentMode->init(clock);
      return TRUE;
    }
  return FALSE;
}





void txFunctions::sendTestPattern()
{
  if(patternNumber==TPBURST) syncBurst();
}



/** send a burst of syncs */
/*
void txFunctions::syncBurst()
{
  // we will send 5msec 1200Hz followed by 50 msec silence
//	synthesPtr->sendTone(0.005,1200.);
  for (int i=300;i<3000;i++)
    {
      synthesPtr->sendTone(0.005,(double) i);
    }
  synthesPtr->sendSilence(0.100);;
}
*/

/** send a burst of syncs */
void txFunctions::syncBurst()
{
  // send 5msec 1200Hz followed by 50 msec silence
//	synthesPtr->sendTone(0.005,1200.);
  for (int i=0;i<50;i++)
    {
      synthesPtr->sendTone(0.005,1200,0,TRUE);
      synthesPtr->sendTone(0.020,1700,0,TRUE);
    }
  synthesPtr->sendSilence(0.100);;
}

void txFunctions::faxStart()
{
  // send 300Hz modulated 1500Hz/2300Hz tone
//	synthesPtr->sendTone(0.005,1200.);
  for (int i=0;i<100;i++)
    {
      synthesPtr->sendTone(0.0333,1500,0,TRUE);
      synthesPtr->sendTone(0.0333,2300,0,TRUE);
    }
  synthesPtr->sendSilence(0.100);;
}




void txFunctions::startTestPattern(uint tpnum)
{
  patternNumber=tpnum;
  txState=TXTEST;
}


//void txFunctions:: initSSTVImage()
//{
//  create(modeIndexTx,txClock);
//}

void txFunctions:: sendPreamble()
{
  addToLog("txFunc:sendPreamble",LOGTXFUNC);
  if(useVOX) synthesPtr->sendTone(1.,1700.,0,FALSE);
  synthesPtr->sendTone(0.1,1900.,0,TRUE);
  synthesPtr->sendTone(0.1,1500.,0,TRUE);
  synthesPtr->sendTone(0.1,1900.,0,TRUE);
  synthesPtr->sendTone(0.1,1500.,0,TRUE);
  synthesPtr->sendTone(0.1,2300.,0,TRUE);
  synthesPtr->sendTone(0.1,1500.,0,TRUE);
  synthesPtr->sendTone(0.1,2300.,0,TRUE);
  synthesPtr->sendTone(0.1,1500.,0,TRUE);
  synthesPtr->sendTone(0.3,1900.,0,TRUE);
  synthesPtr->sendTone(0.01,1200.,0,TRUE);
  synthesPtr->sendTone(0.3,1900.,0,TRUE);
}



void txFunctions:: sendVIS()
{
  int i,l;
  int t=txSSTVParam.VISCode;
  addToLog("txFunc:sendVis",LOGTXFUNC);
  if ((t&0xFF)==0x23) l=16;
  else l=8;
  synthesPtr->sendTone(0.030,1200,0,FALSE); // startbit
  for (i=0;i<l;i++)
    {
      if((t&1)==1) synthesPtr->sendTone(0.030,1100,0,TRUE);
      else synthesPtr->sendTone(0.030,1300,0,TRUE);
      t>>=1;
    }
  synthesPtr->sendTone(0.030,1200,0,TRUE); // stopbit
}


void txFunctions::sendCW()
{
  if(!useCW) return;
  addToLog("txFunc:sendCW",LOGTXFUNC);
  float tone;
  float duration;
  initCW(cwText);
  synthesPtr->sendSilence(0.5);
  while(sendTextCW(tone,duration))
    {
      synthesPtr->sendTone(duration,tone,0,TRUE);
    }
}

void txFunctions::waitEnd()
{
  synthesPtr->sendTone(SILENCEDELAY,00,0,FALSE); // send silence
  addToLog("waitEnd() posting endTXImage",LOGTXFUNC);
  endImageTXEvent *ce=new endImageTXEvent;
  QApplication::postEvent(dispatcherPtr, ce );  // Qt will delete it when done
}

double txFunctions::calcTxTime(bool isDRM,int overheadTime)
{
  double tim=0;
  float tone;
  float duration;
  tim= soundIOPtr->getPlaybackStartupTime();
  tim+=SILENCEDELAY;
  if(!isDRM)
    {
      initializeSSTVParametersIndex(modeIndexTx,TRUE);
      int t=txSSTVParam.VISCode;
      initCW(cwText);
      tim+=1.41; //preamble;
      if ((t&0xFF)==0x23) tim+=18.*0.03;
      else tim+=10.*0.03;
      tim+=txSSTVParam.imageTime;
      tim+=0.5 ;//CW silence gap
      if(useCW)
        {
          while(sendTextCW(tone,duration))
            {
              tim+=duration;
            }
        }
      tim+=0.3; // trailer;
    }
  else
    {
      tim+=overheadTime;
      tim+=txDRM->getDuration();
    }
  addToLog(QString("txFunc: calcTimeTx %1").arg(tim),LOGTXFUNC);
  return tim;
}




void txFunctions::sendImage()
{
  addToLog("txFunc: sendImage",LOGTXFUNC);
  currentMode->transmitImage(txWidgetPtr->getImageViewerPtr());
}

void txFunctions::initDRMImage()
{
  eRSType rsType;
  reedSolomonCoder rsd;
  // we need to save it as a jpg file
  QFileInfo finf(txWidgetPtr->getImageViewerPtr()->getFilename());
  QString fn(QDateTime::currentDateTime().toUTC().toString("yyyyMMddHHmmss"));
  fn+="-"+finf.baseName();
  fixBlockList.clear();

  if(txList.count()>5) txList.removeFirst();
  txList.append(txSession());
  txList.last().drmParams=drmTxParameters;
  txList.last().filename=fn;
  txList.last().extension="jp2";
  txWidgetPtr->getImageViewerPtr()->save(&(txList.last().ba)); // writes image into ba in JP2 format
  rsType=(eRSType)txList.last().drmParams.readSolomon;
  baDRM=txList.last().ba;

  if(rsType!=RSTNONE)
    {
      rsd.encode(baDRM,txList.last().extension,rsType);
      txDRM->init(&baDRM,txList.last().filename,rsTypeStr[rsType],txList.last().drmParams);
    }
  else
    {
      txDRM->init(&baDRM,txList.last().filename,txList.last().extension,txList.last().drmParams);
    }
  // transportID is set
  txList.last().transportID=txTransportID;
}


void txFunctions::initDRMBSR(QByteArray *ba)
{
  baDRM=*ba;
  fixBlockList.clear();
  txDRM->init(&baDRM,"bsr","bin",drmTxParameters);
  addToLog(QString("bsr.bin send").arg(baDRM.size()),LOGPERFORM);
}

bool txFunctions::initDRMFIX(QString fileName,QString extension,eRSType rsType,int mode)
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

bool txFunctions::initDRMFIX(txSession *sessionPtr)
{
  reedSolomonCoder rsd;
  eRSType rsType;
  rsType=(eRSType)sessionPtr->drmParams.readSolomon;
  baDRM=sessionPtr->ba;
  if(rsType!=RSTNONE)
    {
      rsd.encode(baDRM,sessionPtr->extension,rsType);
      txDRM->init(&baDRM,sessionPtr->filename,rsTypeStr[rsType],sessionPtr->drmParams);
    }
  else
    {
      txDRM->init(&baDRM,sessionPtr->filename,sessionPtr->extension,sessionPtr->drmParams);
    }
  return TRUE;
}


void txFunctions::sendDRMFIX()
{
//  QByteArray ba;
//  QFile fi(rxImagesPath+"/bsr.bin");
//  if(fi.open(QIODevice::ReadOnly)<=0) return;
//  ba=fi.readAll();
//  txDRM->init(&ba,"bsr","bin",drmTxParameters);
//  addToLog(QString("bsr.bin send").arg(ba.size()),LOGPERFORM);
//  txDRM->start();
}


void txFunctions::logStatus()
{
  QString stat,statr;
  if(isRunning()) statr="TRUE"; else statr="FALSE";
  addToLog(QString("TX Is running %1").arg(statr),LOGDISPAT);
  stat="TX state: ";
  switch (txState)
    {
    case TXIDLE: stat+="IDLE";  break;
    case TXACTIVE: stat+="ACTIVE";  break;
    case TXSENDTONE: stat+="SENDTONE";  break;
    case TXSENDID: stat+="SENDID";  break;
    case TXSENDDRM: stat+="SENDDRM";  break;
    case TXSENDDRMBSR: stat+="SENDDRMBSR";  break;
    case TXSENDDRMFIX: stat+="SENDDRMFIX";  break;
    case TXSSTVIMAGE: stat+="SSTVIMAGE";  break;
    case TXSSTVPOST: stat+="SSTVPOST";  break;
    case TXFAXSTART: stat+="FAXSTART";  break;
    case TXTEST: stat+="TEST";  break;
    }
  addToLog(stat,LOGTXFUNC);
}

txSession *txFunctions::getSessionPtr(uint transportID)
{
  int i;
  for(i=0;i<txList.count();i++)
    {
      if(txList.at(i).transportID==transportID)
        {
          return &txList[i];
        }
    }
  return NULL;
}





