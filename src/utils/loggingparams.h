#ifndef LOGGINGPARAMS_H
#define LOGGINGPARAMS_H
#include <QString>

//debuglevels
#define LOGALL       99
#define LOGGALLERY   0
#define LOGPARAM     1
#define LOGFFT       2
#define LOGWAVIO     3
#define LOGSYNTHES   4
#define LOGDISPAT    5
#define LOGSOUND     6
#define LOGRXFUNC    7
#define LOGTXFUNC    8
#define LOGSYNC1     9
#define LOGSYNC2     10
#define LOGSLANT     11
#define LOGMODES     12
#define LOGFTP       13
#define LOGIMAG      14
#define LOGSCOPE     15
#define LOGCAM       16
#define LOGRXMAIN    17
#define LOGTXMAIN    18
#define LOGEDIT      19
#define LOGRIGCTRL   20
#define LOGDRMDEMOD  21
#define LOGDRMSRC    22
#define LOGDRMMOT    23
#define LOGDRMTX     24
#define LOGDRMTXMOT  25
#define LOGDRMTXCOD  26
#define LOGDRMTXAAA  27
#define LOGPERFORM   28
#define NUMDEBUGLEVELS (LOGPERFORM+1)

extern QString levelStr[NUMDEBUGLEVELS];
#endif // LOGGINGPARAMS_H
