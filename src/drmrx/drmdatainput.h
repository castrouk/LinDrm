#ifndef DRMDATAINPUT_H
#define DRMDATAINPUT_H
// #include "externs.h"
#include "qsstvdefs.h"
#include "drmrx/drmdefs.h"
#include "sound/resampler.h"

#define RESAMPLINGLEN 2048

class filter;
class downsampleFilter;

class drmDataInput
{
public:
  drmDataInput(int blockSize);
  ~drmDataInput();
//  void doResample(DSPFLOAT rRation,short int *inputBuffer);
  unsigned int getData(DSPFLOAT *inputBuf, DSPFLOAT *outputBuf, DSPFLOAT rRation);
  void init(int blockSize);

private:
//  downsampleFilter *downFilter;
  filter *iqFilter;
  resampler inputResampler;
  int iterationCounter;
};

#endif // DRMDATAINPUT_H
