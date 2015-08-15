#include "csoundout.h"
// #include "qsstvglobal.h"
#define DRM
#include "externs.h"
#include "dsp/synthes.h"
#include "utils/vector.h"
#include <QDebug>

CSoundOut::CSoundOut()
{
}

CSoundOut::~CSoundOut()
{
}


//void CSoundOut::Init(int iNewBufferSize, bool bNewBlocking)
//{
//}

bool CSoundOut::Write(CVector< _SAMPLE >& psData)
{
    //  addToLog(QString("csize start %1").arg(psData.Size()/2),LOGSOUND);
    synthesPtr->writeBuffer((quint32 *)&psData[0],psData.Size()/2);
    //  addToLog(QString("csize end %1").arg(psData.Size()/2),LOGSOUND);
    return FALSE;
}
