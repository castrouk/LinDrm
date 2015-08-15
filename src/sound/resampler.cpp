/**************************************************************************
*   Copyright (C) 2000-2012 by Johan Maes                                 *
*   on4qz@telenet.be                                                      *
*   http://users.telenet.be/on4qz                                         *
*                                                                         *
*   Based on software from                                                *
*   Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik   *
*   Author(s):                                                            *
*   Volker Fischer                                                        *
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
#include "resampler.h"
// #include "qsstvglobal.h"
// #include "sound/soundin.h"

/**
 * @brief Resample routine for arbitrary sample-rate conversions
 *
 * Resample routine for arbitrary sample-rate conversions in a low range (for
 * frequency offset correction).
 * The algorithm is based on a polyphase structure. We upsample the input
 * signal with a factor INTERP_DECIM_I_D and calculate two successive samples
 * whereby we perform a linear interpolation between these two samples to get
 * an arbitraty sample grid.
 *
 * @param prInput input buffer data (e.g. soundcard)
 * @param prOutput output buffer (resampled data)
 * @param rRation  conversion ratio
 * @return int
 */

resampler::resampler()
{
  resamplerBuffer=NULL;
}

resampler::~resampler()
{
  if(resamplerBuffer!=NULL) delete  resamplerBuffer;
}


/**
 * @brief resample the audio data
 *
 * When this function is called, there is already enough data in the sound carad buffers
 *
 * @param rRation the resample ratio
 */
void resampler::resample(DSPFLOAT rRation, DSPFLOAT *inputBuffer)
{
  unsigned int i;

  /* Move old data from the end to the history part of the buffer and add new data (shift register) */
  for (i = 0; i < iHistorySize; i++) resamplerBuffer[i] = resamplerBuffer[i + iInputBlockSize];
  for (i = 0; i < iInputBlockSize; i++)  resamplerBuffer[i + iHistorySize] = inputBuffer[i];

  /* Sample-interval of new sample frequency in relation to interpolated  sample-interval */
  rTStep = (DSPFLOAT) INTERP_DECIM_I_D / rRation;

	/* Init output counter */
	int im = 0;
//    if (input_samples_buffer_request == 0)	/* no resampling needed, just copy */

      {
        im = iInputBlockSize;
        for (i = 0; i < iInputBlockSize; i++) rxBuffer.put(resamplerBuffer[i + iHistorySize]);
        return ;
      }

	/* Main loop */
	do
	{
		/* Quantize output-time to interpolated time-index */
    const int ik = (int) rtOut;


		/* Calculate convolutions for the two interpolation-taps ------------ */
		/* Phase for the linear interpolation-taps */
		const int ip1 = ik % INTERP_DECIM_I_D;
		const int ip2 = (ik + 1) % INTERP_DECIM_I_D;

		/* Sample positions in input vector */
		const int in1 = (int) (ik / INTERP_DECIM_I_D);
		const int in2 = (int) ((ik + 1) / INTERP_DECIM_I_D);

		/* Convolution */
    DSPFLOAT ry1 = (DSPFLOAT) 0.0;
    DSPFLOAT ry2 = (DSPFLOAT) 0.0;
		for (int i = 0; i < RES_FILT_NUM_TAPS_PER_PHASE; i++)
		{
      ry1 += fResTaps1To1[ip1][i] * resamplerBuffer[in1 - i];
      ry2 += fResTaps1To1[ip2][i] * resamplerBuffer[in2 - i];
		}


		/* Linear interpolation --------------------------------------------- */
		/* Get numbers after the comma */
    const DSPFLOAT rxInt = rtOut - (int) rtOut;
    rxBuffer.put((ry2 - ry1) * rxInt + ry1);
		/* Increase output counter */
		im++;

		/* Increase output-time and index one step */
		rtOut = rtOut + rTStep;
	} 
	while (rtOut < rBlockDuration);

	/* Set rtOut back */
	rtOut -= iInputBlockSize * INTERP_DECIM_I_D;
}

void resampler::init(int blocksize)
{
  unsigned int i;
  iInputBlockSize = blocksize;

  /* History size must be one sample larger, because we use always TWO convolutions */
	iHistorySize = RES_FILT_NUM_TAPS_PER_PHASE + 1;

	/* Calculate block duration */
  rBlockDuration = (iInputBlockSize + RES_FILT_NUM_TAPS_PER_PHASE) * INTERP_DECIM_I_D;

	/* Allocate memory for internal buffer, clear sample history */
  resamplerBuffer= new DSPFLOAT[iInputBlockSize + iHistorySize];
  for(i=0;i<(iInputBlockSize + iHistorySize);i++) resamplerBuffer[i]= 0;
	/* Init absolute time for output stream (at the end of the history part) */
  rtOut = (DSPFLOAT) RES_FILT_NUM_TAPS_PER_PHASE * INTERP_DECIM_I_D;
  rxBuffer.reset();
}


