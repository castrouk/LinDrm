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
#ifndef TXFUNC_H
#define TXFUNC_H
#include <QThread>
#include "dsp/filterparam.h"
#include "sstv/sstvparam.h"
#include "sstv/modes/modebase.h"
#include "drmtx/drmtransmitter.h"
#include "utils/reedsolomoncoder.h"

class imageCoder;

#define TPBURST 0

extern esstvMode modeIndexTx;
extern int templateIndex;
extern bool useTemplate;
extern bool useCW;
extern bool useVOX;

class drmTransmitter;

struct txSession
{
  drmTxParams drmParams;
  QByteArray ba;  // contains the image data in jpg, jp2 .... format
  uint transportID;
  QString filename;
  QString extension;
};

class txFunctions: public QThread
{
public:
  enum etxState
    {
    TXIDLE,	//!< in idle loop
    TXACTIVE,
    TXSENDTONE,
    TXSENDID,
    TXSENDDRM,
    TXSENDDRMBSR,
    TXSENDDRMFIX,
    TXSSTVIMAGE,
    TXSSTVPOST,
    TXFAXSTART,
    TXTEST
    };
public:
  txFunctions();
  ~txFunctions();
  bool create(esstvMode m,DSPFLOAT clock);
  modeBase *currentMode;
  void run();
  void stopAndWait();
  bool hasStarted() {return started;}
  void setTXState(etxState d) { txState=d;}
  etxState getTXState() { return txState;}
  void syncBurst();
  void faxStart();
  void setDRMTxParams(drmTxParams params)
  {
    drmTxParameters=params;
  }
  txSession *getSessionPtr(uint transportID);

  void enableNoise(bool en,double level=0.)
  {
    if (level==0)
      {
        noiseEnabled=en;
      }
  }
  void setFilter(efilterType filterType);
  void startTestPattern(uint tpnum);
  //  void setCall(const QString &str);
  //  void setToOperator(const QString &str);
  //  void setRSV(const QString &str);
  void setToneParam(double duration,double lowerFreq,double upperFreq=0)
  {
    toneDuration=duration;
    toneLowerFrequency=lowerFreq;
    toneUpperFrequency=upperFreq;
  }
  double calcTxTime(bool isDRM, int  overheadTime);
  void logStatus();
  void initDRMBSR(QByteArray *ba);
  bool initDRMFIX(QString fileName, QString extension, eRSType rsType, int mode);
  bool initDRMFIX(txSession *sessionPtr);
private:
  bool abort;
  void init();
  unsigned long sampleCounter;
  bool noiseEnabled;

  short int filter(double sample);
  void write(double sample);

  etxState txState;
  uint patternNumber;
  void sendTestPattern();
  void sendPreamble();
  void sendVIS();
  void sendImage();
  void initDRMImage();
  void sendDRMFIX();
  void sendCW();
  //  void initSSTVImage();
  void waitEnd();
  double toneDuration;
  double toneLowerFrequency;
  double toneUpperFrequency;
  esstvMode oldMode;
  bool started;
  drmTransmitter *txDRM;
  drmTxParams drmTxParameters;
  QByteArray baDRM;
  double waterfallTime;
  QList <txSession> txList;
};

#endif


