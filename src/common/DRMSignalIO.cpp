/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
 *  Francesco Lanza
 *
 * Description:
 *	Transmit and receive data
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

#include "DRMSignalIO.h"
#include <QColor>
// #include "TransmitterFilter.h"
// #include "vsattach/attach.h"
// extern Attach *vscope;

#include "soundio.h"

extern QColor Io_Led;
extern QVector<int> audio;

// extern QColor Io_Led;

//short max = 0,min = 0;

/* Implementation *************************************************************/
/******************************************************************************\
* Transmitter                                                                  *
\******************************************************************************/
//void CTransmitData::ProcessDataInternal(CParameter& Parameter)
//{
//  int i;
//  //	max = 0;
//  //	min = 0;
//  //	qWarning("TODO: %s	Line: %d", __FILE__, __LINE__);

//  /* Filtering of output signal (FIR filter) ------------------------------ */
//  /* Transfer input data */
//  for (i = 0; i < iInputBlockSize; i++)
//  {
//    rvecDataReal[i] = (*pvecInputData)[i].real();
//  }

//  filter.process(rvecDataReal);

//  /* Convert vector type. Fill vector with symbols (collect them) */
//  const int iNs2 = iInputBlockSize * 2;
//  for (i = 0; i < iNs2; i += 2)
//  {
//    const int iCurIndex = iBlockCnt * iNs2 + i;

//    /* Imaginary, real */
//    const short sCurOutReal = (short) (rvecDataReal[i / 2] * rNormFactor);

//    //		if (sCurOutReal >= max) max = sCurOutReal;
//    //		if (sCurOutReal <= min) min = sCurOutReal;


//    /* Use real valued signal as output for both sound card channels */
//    vecsDataOut[iCurIndex] = vecsDataOut[iCurIndex + 1] = sCurOutReal; //@
//  }

//  iBlockCnt++;
//  if (iBlockCnt == iNumBlocks)
//  {
//    iBlockCnt = 0;

//    /* Write data to sound card. Must be a blocking function */
//    pSound->Write(vecsDataOut);
//    //			qWarning("TODO: %s	Line: %d", __FILE__, __LINE__);

//  }

//}



//void CTransmitData::SetWrite_to_File(bool yes)
//{

//  qWarning("TODO: %s	Line: %d", __FILE__, __LINE__);
//  //if (yes)
//  //bUseSoundcard = false;
//  //else
//  //bUseSoundcard = true;
//}


//void CTransmitData::InitInternal(CParameter& TransmParam)
//{
//  /* Init vector for storing a complete DRM frame number of OFDM symbols */
//  iBlockCnt = 0;
//  iNumBlocks = TransmParam.iNumSymPerFrame;
//  //	qWarning("TODO: %s	%d", __FILE__, __LINE__);
//  const int iTotalSize =
//      TransmParam.iSymbolBlockSize * 2 /* Stereo */ * iNumBlocks; //@
//  //	qWarning("TODO: %s	%d", __FILE__, __LINE__);
//  iBigBlockSize = iTotalSize;
//  vecsDataOut.resize(iTotalSize);

//  /* Init sound interface */
//  //	qWarning("TODO: %s	%d", __FILE__, __LINE__);
//  pSound->Init(iTotalSize, TRUE);

//  /* Choose correct filter for chosen DRM bandwidth. Also, adjust offset
//	   frequency for different modes. E.g., 5 kHz mode is on the right side
//	   of the DC frequency */
//  float* pCurFilt;
//  _REAL rNormCurFreqOffset;

//  switch (TransmParam.GetSpectrumOccup())
//  {
//  case SO_0:
//    pCurFilt = fTransmFilt4_5;

//    /* Completely on the right side of DC */
//    rNormCurFreqOffset =
//	(rDefCarOffset + (_REAL) 2250.0) / SOUNDCRD_SAMPLE_RATE;
//    break;

//  case SO_1:
//    pCurFilt = fTransmFilt5;

//    /* Completely on the right side of DC */
//    rNormCurFreqOffset =
//	(rDefCarOffset + (_REAL) 2500.0) / SOUNDCRD_SAMPLE_RATE;
//    break;
//  }

//  /* Modulate filter to shift it to the correct IF frequency */
//  vector<_REAL> coeffs(NUM_TAPS_TRANSMFILTER);
//  for (int i = 0; i < NUM_TAPS_TRANSMFILTER; i++)
//    coeffs[i] = pCurFilt[i] * cos(2.0 * M_PI * rNormCurFreqOffset * i);

//  filter.set_coeffs(coeffs);

//  rvecDataReal.resize(TransmParam.iSymbolBlockSize);

//  /* All robustness modes and spectrum occupancies should have the same output
//	   power. Calculate the normaization factor based on the average power of
//	   symbol (the number 3000 was obtained through output tests) */
//  // bei 3000 -> +- 6000
//  // bei 9000 -> +- 20000 achtung, 12000 ist zuviel -> foldback
//  // moeglich +-32000
//  rNormFactor = 6000.0 / sqrt(TransmParam.rAvPowPerSymbol);

//  /* Define block-size for input */
//  iInputBlockSize = TransmParam.iSymbolBlockSize;
//}

//CTransmitData::~CTransmitData()
//{}

#define no_dc_tap 17*2

double the_dcsum = 0.0;
double dcsumbuf[no_dc_tap] = {0.0};
int dcsumbufpt = 0;

_REAL averdc = 0.0;


/******************************************************************************\
* Receive data from the sound card                                             *
\******************************************************************************/
void CReceiveData::ProcessDataInternal(CParameter& Parameter)
{
  int i;
  _REAL temp;
  double dcsum = 0.0;

  //	fprintf(stderr,"Here: iOutputBlockSize: %d ,%s	%d\n",iOutputBlockSize,__FILE__,__LINE__);
  if (bUseSoundcard == TRUE)
  {
    /* Using sound card ------------------------------------------------- */
    /* Get data from sound interface. The read function must be a
		   blocking function! */

    //  		fprintf(stderr,"Here: iOutputBlockSize: %d ,%s	%d\n",iOutputBlockSize,__FILE__,__LINE__);
    // fprintf(stderr,"Here: iBufferSizeIn: %d ,%s	%d\n",iBufferSizeIn,__FILE__,__LINE__);


    //	fprintf(stderr,"pSound->Read:, %s	%d\n",__FILE__,__LINE__);

				if (pSound->Read(vecsSoundBuffer) == false)
				{
      Io_Led = Qt::green;
    }
    else
    {
      Io_Led = Qt::red;
    }

    /* Write data to output buffer */
    for (i = 0; i < iOutputBlockSize; i++)
    {
#ifdef MIX_INPUT_CHANNELS
      /* Mix left and right channel together. Prevent overflow! First,
			   copy recorded data from "short" in "int" type variables */
      const int iLeftChan = vecsSoundBuffer[2 * i];
      const int iRightChan = vecsSoundBuffer[2 * i + 1];

      temp = (_REAL) ((iLeftChan + iRightChan) / 2); //@
      dcsum += temp;
      (*pvecOutputData)[i] = temp - averdc;
#else
      /* Use only desired channel, chosen by "RECORDING_CHANNEL" */
      (*pvecOutputData)[i] =
	  (_REAL) vecsSoundBuffer[2 * i + RECORDING_CHANNEL];
#endif
    }

    //		vscope->SetInt(pvecOutputData,2000,SRC);


    the_dcsum -= dcsumbuf[dcsumbufpt];
    dcsumbufpt++;
    if (dcsumbufpt >= no_dc_tap) dcsumbufpt = 0;
    dcsumbuf[dcsumbufpt] = dcsum / (double)iOutputBlockSize;
    the_dcsum += dcsumbuf[dcsumbufpt];

    //		fprintf(stderr,"the_dcsum: %f, %s	%d\n",the_dcsum,__FILE__,__LINE__);

    averdc = the_dcsum;
  }
  else
  {
    /* Read data from file ---------------------------------------------- */
    for (i = 0; i < iOutputBlockSize; i++)
    {
      /* If enf-of-file is reached, stop simulation */
      float tIn;

      if (fscanf(pFileReceiver, "%e\n", &tIn) == EOF)
      {
	Parameter.bRunThread = FALSE;

	/* Set output block size to zero to avoid writing invalid
				   data */
	iOutputBlockSize = 0;

	return;
      }
      else
      {
	/* Write internal output buffer */
	(*pvecOutputData)[i] = (_REAL) tIn;
      }
    }
  }


  /* Flip spectrum if necessary ------------------------------------------- */
  if (bFippedSpectrum == TRUE)
  {
    static _BOOLEAN bFlagInv = FALSE;

    for (i = 0; i < iOutputBlockSize; i++)
    {
      /* We flip the spectrum by using the mirror spectrum at the negative
			   frequencys. If we shift by half of the sample frequency, we can
			   do the shift without the need of a Hilbert transformation */
      if (bFlagInv == FALSE)
      {
	(*pvecOutputData)[i] = -(*pvecOutputData)[i];
	bFlagInv = TRUE;
      }
      else
	bFlagInv = FALSE;
    }
  }


  /* Copy data in buffer for spectrum calculation ------------------------- */
  vecrInpData.AddEnd((*pvecOutputData), iOutputBlockSize);


  /* Update level meter */
  SignalLevelMeter.Update((*pvecOutputData));
}

// CRealVector		vecrHammingWindow;

#define LEN_SPEC 512
#define LEN_DS_INPUT_VECTOR 2048

void CReceiveData::InitInternal(CParameter& Parameter)
{
  /* Init sound interface. Set it to one symbol. The sound card interface
	   has to taken care about the buffering data of a whole MSC block.
	   Use stereo input (* 2) */
  //	pSound->InitRecording(Parameter.iSymbolBlockSize * 2);
  pSound->Init(Parameter.iSymbolBlockSize * 2);

  /* Init buffer size for taking stereo input */
  vecsSoundBuffer.resize(Parameter.iSymbolBlockSize * 2);

  /* Init signal meter */
  SignalLevelMeter.Init(0);

  /* Define output block-size */
  iOutputBlockSize = Parameter.iSymbolBlockSize;

  //	const int iLenDSInputVector = 2048;
  vecrHammingWindow.resize(LEN_DS_INPUT_VECTOR);
  make_hamming_vec(vecrHammingWindow);

  fft.init(LEN_DS_INPUT_VECTOR);
}

CReceiveData::~CReceiveData()
{
  /* Close file (if opened) */
  if (pFileReceiver != NULL)
    fclose(pFileReceiver);
}

void CReceiveData::GetInputSpec(vector<_REAL>& vecrData)
{

  int				i;
  //	CComplexVector	veccSpectrum;
  //	CRealVector		vecrFFTInput;
  //	_REAL			rNormSqMag;
  _REAL			rNormData;

  const int iLenInputVector = 2 * 2048;
  const int iLenDSInputVector = 2048;
  //	const int iLenSpec = 512;

  //	std::cerr << "CReceiveData::GetInputSpec\n";

  /* Init input and output vectors */
  vecrData.resize(LEN_SPEC, (_REAL) 0.0);

  /* Lock resources */
  Lock();

  rNormData = 0.2 * (_REAL)iLenInputVector * iLenInputVector * _MAXSHORT;

  //	veccSpectrum.Init(iLenSpec);

  /* Copy data from shift register */
  //	vecrFFTInput.Init(iLenDSInputVector);

  for (i = 0; i < iLenDSInputVector; i++)
  {
    fft.input()[i] = vecrInpData[i*2] * vecrHammingWindow[i];
  }


  /* Get spectrum */
  //	veccSpectrum = rfft(vecrFFTInput);
  fft.execute();

  /* Log power spectrum data */
  for (i = 0; i < LEN_SPEC; i++)
  {
    //	rNormSqMag = SqMag(veccSpectrum[i]) / rNormData;
    vecrData[i] = sqrt(norm(fft.output()[i]) / rNormData);
  }

  /* Release resources */
  Unlock();
}


/* Level meter -------------------------------------------------------------- */
void CSignalLevelMeter::Update(_REAL rVal)
{
  /* Search for max */
  const _REAL rCurAbsVal = abs(rVal);
  if (rCurAbsVal > rCurLevel)
    rCurLevel = rCurAbsVal;
}


void CSignalLevelMeter::Update(vector<_REAL> vecrVal)
{
  /* Do the update for entire vector */
  const int iVecSize = vecrVal.size();
  rCurLevel = 0;
  for (int i = 0; i < iVecSize; i += 2)
    Update(vecrVal[i]);
}

void CSignalLevelMeter::Update(vector<_SAMPLE> vecsVal)
{
  /* Do the update for entire vector, convert to real */
  const int iVecSize = vecsVal.size();
  rCurLevel = 0;
  for (int i = 0; i < iVecSize; i += 2)
    Update((_REAL) vecsVal[i]);
}

_REAL CSignalLevelMeter::Level()
{
  return rCurLevel / _MAXSHORT;
}
