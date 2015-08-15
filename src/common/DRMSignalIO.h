/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
 *
 * Description:
 *	See DRMSignalIO.cpp
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#if !defined(DRMSIGNALIO_H__3B0BA660_CA63_4344_B_23E7A0D31912__INCLUDED_)
#define DRMSIGNALIO_H__3B0BA660_CA63_4344_B_23E7A0D31912__INCLUDED_

#include "GlobalDefinitions.h"

#include "Parameter.h"
#include "Modul.h"
#include <math.h>
// #include "matlib/Matlib.h"
#include "maths-utils/Filters.h"
#include "maths-utils/Functions.h"
#include "maths-utils/FFT.h"
// #include "TransmitterFilter.h"
#include <sndfile.hh>
#include <iostream>
#include <cmath>

#include <vector>
#include <QVector>

// #include "../sound/Sound.h"
//#include "sound/drm_portaudio.h"
//#include "sound.h"
#include "soundinterface.h"

#include <QObject>

/* Definitions ****************************************************************/
#define	METER_FLY_BACK				15

/* Length of vector for input spectrum. We use approx. 0.2 sec
   of sampled data for spectrum calculation, this is 2^13 = 8192 to
   make the FFT work more efficient */
#define NUM_SMPLS_4_INPUT_SPECTRUM	8192

/* Use raw 16 bit data or in text form for file format for DRM data. Defining
   the following macro will enable the raw data option */
// #define FILE_DRM_USING_RAW_DATA

/* If this flag is defined, both input channels are mixed together, therefore
   no right or left channel choice must be made */
#define MIX_INPUT_CHANNELS

/* Chose recording channel: 0: Left, 1: Right, disabled if previous flag is
   set! */
#define RECORDING_CHANNEL			0


/* Classes ********************************************************************/

class CSignalLevelMeter
{


public:
  CSignalLevelMeter() : rCurLevel((_REAL) 0.0)
  {}
  virtual ~CSignalLevelMeter()
  {}

  void Init(_REAL rStartVal)
  {
    rCurLevel = abs(rStartVal);
  }
  void Update(_REAL rVal);
  void Update(vector<_REAL> vecrVal);
  void Update(vector<_SAMPLE> vecsVal);
  _REAL Level();


protected:
  _REAL rCurLevel;
};


class CTransmitData : public CTransmitterModul<_COMPLEX, _COMPLEX>
{
public:
  enum EOutFormat {OF_REAL_VAL /* real valued */, OF_IQ /* I / Q */,
		   OF_EP /* envelope / phase */};

#ifdef WRITE_TRNSM_TO_FILE
  CTransmitData() : pFileTransmitter(NULL),
    rDefCarOffset((_REAL) VIRTUAL_INTERMED_FREQ)
  {}
  void SetIQOutput(const EOutFormat eFormat)
  {} /* Not used in file mode */
  _BOOLEAN GetIQOutput()
  {
    return FALSE;
  }
#else
  //	CTransmitData(CSoundOut* pNS)
  CTransmitData(CSoundOutInterface* pNS) :
    strOutFileName("test/TransmittedData.txt"),
    bUseSoundcard(TRUE),
    pSound(pNS),
    eOutputFormat(OF_REAL_VAL),
    rDefCarOffset((_REAL) VIRTUAL_INTERMED_FREQ)
  {}
  void SetIQOutput(const EOutFormat eFormat)
  {
    eOutputFormat = eFormat;
  }
  EOutFormat GetIQOutput()
  {
    return eOutputFormat;
  }


  //	void SetWrite_to_File(const string strNFN, bool yes);
  void SetWrite_to_File(bool yes);


#endif
  virtual ~CTransmitData();

  void SetCarOffset(const _REAL rNewCarOffset)
  {
    rDefCarOffset = rNewCarOffset;
  }

protected:
#ifdef WRITE_TRNSM_TO_FILE
  FILE*			pFileTransmitter;
#else
  FILE*				pFileTransmitter;
  SndfileHandle *outfile;
  string				strOutFileName;
  _BOOLEAN			bUseSoundcard;
  //	CSoundOut*			oSound;
  CSoundOutInterface*	pSound;



  vector<short>	vecsDataOut;
  int				iBlockCnt;
  int				iNumBlocks;
  EOutFormat		eOutputFormat;
#endif

  FIRFilter<_REAL>	filter;

  _REAL			rDefCarOffset;
  vector<_REAL>		rvecDataReal;

  _REAL			rNormFactor;
  int					iBigBlockSize;

  virtual void InitInternal(CParameter& TransmParam);
  virtual void ProcessDataInternal(CParameter& Parameter);
};

class CReceiveData : public QObject, public CReceiverModul<_REAL, _REAL>
{
  Q_OBJECT
public:
  CReceiveData(CSoundInInterface* pNS) : pFileReceiver(NULL), pSound(pNS),
    vecrInpData(NUM_SMPLS_4_INPUT_SPECTRUM, (_REAL) 0.0), bFippedSpectrum(FALSE),
    bUseSoundcard(TRUE)//, bNewUseSoundcard(TRUE)
  {
    qWarning("TODO: %s	Line: %d", __FILE__, __LINE__);
  }

  virtual ~CReceiveData();

  _REAL GetLevelMeter()
  {
    return SignalLevelMeter.Level();
  }
  void GetInputSpec(vector<_REAL>& vecrData);

  void SetFlippedSpectrum(const _BOOLEAN bNewF)
  {
    bFippedSpectrum = bNewF;
  }
  _BOOLEAN GetFlippedSpectrum()
  {
    return bFippedSpectrum;
  }

  void SetUseSoundcard(const _BOOLEAN bNewUS)
  {
    bUseSoundcard = bNewUS;
    SetInitFlag();
  }

  void SetSoundInterface(CSoundInInterface* pS) { pSound = pS;}
  //	void SetInChanSel(const EInChanSel eNS) {eInChanSelection = eNS;}


signals:
  void led_IO(int led,int v);

protected:
  CSignalLevelMeter		SignalLevelMeter;

  FILE*					pFileReceiver;

  //	CSoundIn*					iSound;
  CSoundInInterface*		pSound;

  //	CSoundOut*					oSound;
  vector<_SAMPLE>		vecsSoundBuffer;

  GShiftRegister<_REAL>	vecrInpData;
  vector<_REAL> vecrHammingWindow;
  FFTr2c fft;

  _BOOLEAN				bFippedSpectrum;
  _BOOLEAN				bUseSoundcard;
  //	_BOOLEAN				bNewUseSoundcard;

  virtual void InitInternal(CParameter& ReceiverParam);
  virtual void ProcessDataInternal(CParameter& ReceiverParam);
};


#endif // !defined(DRMSIGNALIO_H__3B0BA660_CA63_4344_B_23E7A0D31912__INCLUDED_)
