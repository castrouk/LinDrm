#ifndef QSSTVDEFS_H
#define QSSTVDEFS_H

#include <stdint.h>
#include <complex>
using namespace std; /* Because of the library: "complex" */

#define RXSTRIPE 1024
#define TXSTRIPE 1024
#define SUBSAMPLINGRATIO 4

#define SAMPLINGSTRIPE (SUBSAMPLINGRATIO*RXSTRIPE) // 4 times oversampled

#define MAXNUMCHANNELS 2
#define SOUNDFRAME  quint32
#define BASESAMPLERATE 48000.
#define SAMPLERATE (BASESAMPLERATE/SUBSAMPLINGRATIO)


//typedef double DSPFLOAT;
typedef float DSPFLOAT;
typedef unsigned char byte;
// enum etransmissionMode {SSTV,DRM,NOMODE};

/* Define the application specific data-types ------------------------------- */
typedef	double	_REAL;
typedef	complex<_REAL>	_COMPLEX;
typedef short _SAMPLE;
typedef unsigned char	_BYTE;
typedef bool	_BOOLEAN;
typedef unsigned char _BINARY;

#ifndef FALSE
#define FALSE false
#define TRUE true
#endif

#endif // QSSTVDEFS_H
