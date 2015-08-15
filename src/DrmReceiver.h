#ifndef DRMRECEIVER_H
#define DRMRECEIVER_H

#include <QThread>
#include <QStringList>
#include <QSettings>
#include "sound/sound.h"


#include "qsstvdefs.h"
#include "utils/buffermanag.h"
#include "drmrx/drmdefs.h"
#include "drmrx/drmdatainput.h"
#include "drmrx/sourcedecoder.h"
#include "common/maths-utils/FFT.h"
#include "common/GVector.h"
#include <QMutex>
#include "drmtx/common/util/Modul.h"
#include "soundinterface.h"
#include <sndfile.hh>
#include "util/Modul.h"
#include "util/Vector.h"

class filter;
class downsampleFilter;
class modeBase;
class demodulator;



#define PERIODBITS 12
#define PERIODSIZE (1<<PERIODBITS)
#define BUFFERSIZE (8*PERIODSIZE)
const int BufferSize = PERIODSIZE * 2;



class DRMReceiver : public QThread
{
	Q_OBJECT
public:
	DRMReceiver();
	virtual ~DRMReceiver();
	void	Start(bool);
//	bool init();
//	void init_snd();
	void setDevName(QString s)
	{
		devName = s;
		pSoundInInterface->SetDev(devName.toStdString());
	}
	bool getAck(){return m_stopAck;}

	QString getDevName()
	{
		QString s(pSoundInInterface->GetDev().c_str());
		return s;
	}

	CSoundInInterface*		GetSoundInInterface() {return pSoundInInterface;}

	void runDRM();
	bool GetInputSpec(vector<_REAL>& );
	void GetVectorSpace(vector<_COMPLEX>& veccData);
	QString getCall();
	vector<_REAL> * getSpec(){return &vecrData;}

	void ReReadSoundINdev()
	{
		GetSoundin();
		pSoundInInterface->SetDev(devName.toStdString());
	}

	void GetSoundin();
	void openSoundFile(QString, bool);
	bool fRead(CVector<_SAMPLE>& psData);



protected:
	/* Modules */
	CSoundInInterface*		pSoundInInterface;
//	GVectorEx<TInput>*	pvecInputData;
	vector<_COMPLEX>	vecSigSpacBuf;

private slots:
	void run();
	void DrmInit();

private:
	QStringList card_names;
	QStringList card_descriptions;
	QString devName;
	bool isStereo;
	CVector<_SAMPLE>	vecsSoundBuffer;
	short int tempBuf[SAMPLINGSTRIPE];
	bool m_run;
	bool m_stopAck;

	///////////////////////////////////

	filter *rxFilter;
	filter *syncFilter;
	downsampleFilter *m_downSampleFilter;
	unsigned int m_sampleCounter;
	unsigned int m_bufferCounter;
	buffer<DSPFLOAT,22> m_rxHoldingBuffer;  // 2^22= 4194304, divided by samplingrate 12000 gives 349 seconds buffering
	buffer<int,22> demodBuffer;  // 2^22= 4194304, divided by samplingrate 12000 gives 349 seconds buffering
	float m_rRation;
	int m_n;
	int m_im;
	drmDataInput *m_dataInputPtr;
	//	 bool m_run;
	//	 bool m_stopAck;
	float resamp_signal[2 * DRMBUFSIZE];
	QMutex				mutex;
	FFTr2c fft;
	//		GShiftRegister<_REAL>	vecrInpData;
	GVectorEx<_REAL>	vecrInpData;
	vector<_REAL> vecrHammingWindow;
	vector<_REAL>		vecrData;
	vector<_REAL>		tvecrData;
	double *dataBuffer;
	GVectorEx<_REAL>	pvecInputData;
	GVectorEx<_REAL>	pvecOutputData;
	//		vector<_REAL>*	pvecInputData;
	//		vector<_REAL>*	pvecOutputData;
	QString sfname;
	SNDFILE *sfin;
	SF_INFO sfinfo;
	bool fromFile;
	bool m_loop;
	int iN_mux;
};



#endif // DRMRECEIVER_H
