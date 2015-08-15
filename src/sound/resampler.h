#ifndef RESAMPLER_H
#define RESAMPLER_H

#include "resamplefilter.h"
#include "qsstvdefs.h"
#include "utils/buffermanag.h"


/* Classes ********************************************************************/
class resampler
{
public:
  resampler();
  ~resampler();
  void init(int blocksize);
  void resample(DSPFLOAT rRation, DSPFLOAT *inputBuffer);
  buffer<DSPFLOAT,16> rxBuffer;
protected:
  DSPFLOAT *resamplerBuffer;
  DSPFLOAT rTStep;
  DSPFLOAT rtOut;
  DSPFLOAT rBlockDuration;
  unsigned int	iHistorySize;
  unsigned int iInputBlockSize;

};

#endif
