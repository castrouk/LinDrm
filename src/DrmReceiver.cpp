#include "DrmReceiver.h"
#include <QDebug>
#include <unistd.h>
#include <QThread>
#include "soundcard.h"
#include "sound/sound.h"

#include "qsstvdefs.h"
#include "utils/buffermanag.h"
#include "dsp/filterparam.h"
#include "dsp/filter.h"
#include "dsp/downsamplefilter.h"

#include "drmrx/drmdefs.h"
#include "drmrx/drmdatainput.h"
#include "drmrx/sourcedecoder.h"
#include "drmrx/drm.h"
#include "drmrx/drmproto.h"
#include "drmrx/demodulator.h"
#include <QElapsedTimer>

//#define LEN_SPEC 300
//#define LEN_SPEC 562
//#define LEN_SPEC 512
// #define LEN_SPEC 600

// LEN_SPEC now extern int set to width of spec disp.

#define LEN_DS_INPUT_VECTOR 2048
#define NUM_FAC_CELLS 45


#include <QColor>
extern QColor Io_Led;
extern bool newSamp;



//extern demodulator *demodulatorPtr;

DRMReceiver::DRMReceiver() : pSoundInInterface(new CSoundIn)

{
	fromFile = false;
	qDebug() << __FILE__ << __LINE__;
	//	pSoundInInterface(new CSoundOut);

	//	pSoundInInterface = new CSoundIn;
	qDebug() << __FILE__ << __LINE__;
	m_downSampleFilter=NULL;
	rxFilter = NULL;
	syncFilter = NULL;
	iN_mux = NUM_FAC_CELLS;
	m_dataInputPtr = new drmDataInput(RXSTRIPE);
	demodulatorPtr = new demodulator;
	DrmInit();
}

DRMReceiver::~DRMReceiver()
{
	qDebug() << __FILE__ << __LINE__;
}

void DRMReceiver::GetSoundin()
{
	QSettings settings("G4EKQ", "LinDRM");

	settings.beginGroup("DrmRX");
	{
		devName = settings.value("SoundINdev").toString();
	}
	settings.endGroup();
	//	Snd->SetDev(devName.toStdString());

}

void DRMReceiver::openSoundFile(QString f,bool loop)
{
	if(f.isEmpty())
	{
		fromFile = false;
			return;
	}
	sfname = f;
//	qDebug() << "sfname" << sfname;     /* Used to be called samples. */
	sfin = sf_open(sfname.toLatin1().data(), SFM_READ, &sfinfo);
//	qDebug() << "frames" << sfinfo.frames;     /* Used to be called samples. */
//	qDebug() << "samplerate" << sfinfo.samplerate;
//	qDebug() << "channels" << sfinfo.channels;
//	qDebug() << "format" << sfinfo.format;
//	qDebug() << "sections" << sfinfo.sections;
//	qDebug() << "seekable" << sfinfo.seekable;
	fromFile = true;
	m_loop = loop;
}

void DRMReceiver::DrmInit()
{
	m_n = DRMBUFSIZE;
	/* initialisations */
	demodulatorPtr->init();
	m_dataInputPtr->init(RXSTRIPE);
	initGetmode( m_n / 4);
	m_rRation = 1.000;
	samplerate_offset_estimation = 0.0;
	runstate = RUN_STATE_POWER_ON;		/* POWER_ON */
	channel_decoding();
	runstate = RUN_STATE_INIT;		/* INIT */
	channel_decoding();
	//////////////////////////////////////////////////////////////////////////////////////
	// channel_decoding() only looks for POWER_ON & INIT so FIRST & NORMAL are equal
	//////////////////////////////////////////////////////////////////////////////////////
	runstate = RUN_STATE_FIRST;			/* FIRSTRUN */
//	runstate = RUN_STATE_NORMAL;			/* NORMAL RUN */

	if(m_downSampleFilter==NULL)
	{
		m_downSampleFilter=new downsampleFilter(SAMPLINGSTRIPE,
															 downSampleFilterParam,
															 DSAMPLEFILTERLEN,
															 FALSE);
	}
	else
	{
		m_downSampleFilter->init();
	}
	m_sampleCounter=0;
	m_bufferCounter=0;
	m_rxHoldingBuffer.reset();
	vecrHammingWindow.resize(LEN_DS_INPUT_VECTOR);
	make_hamming_vec(vecrHammingWindow);
	fft.init(LEN_DS_INPUT_VECTOR);

}



void DRMReceiver::Start(bool run)
{
	if(run)
	{
		QSettings settings("G4EKQ", "LinDRM");
		settings.beginGroup("DrmRX");

		devName = settings.value("SoundINdev").toString();
		qDebug() << __FILE__ << __LINE__ << devName;
		pSoundInInterface->SetDev(devName.toStdString());
		pSoundInInterface->Init(48000, BufferSize,true);
		start();
	}
	else
		m_run = false;
}

bool DRMReceiver::fRead(CVector<_SAMPLE>& psData)
{
	msleep(75);
	int len = BufferSize / 2;
	sf_count_t c = sf_readf_short(sfin, &psData[0], len);
	if(c != len)
	{
		if(m_loop)
		{
			/* rewind */
			sf_seek(sfin, 0, SEEK_SET);
			c = sf_readf_short(sfin, &psData[0], len);
		}
		else
			return true;
	}
	return false;
}

void DRMReceiver::run()
{
//	QElapsedTimer Timer;
	m_run = true;
	m_stopAck = false;
	CVector<short> tmpSnd(BufferSize);
	while(m_run)
	{
		if(fromFile)
		{
//			Timer.start();
			fRead(tmpSnd);
			for(int i = 0; i < SAMPLINGSTRIPE; i++)
			{
				tempBuf[i] = tmpSnd.at(i);
			}
		}
		else
		{
//			Timer.start();
			pSoundInInterface->Read(tmpSnd);
			for(int i = 0; i < SAMPLINGSTRIPE; i++)
			{
				tempBuf[i] = tmpSnd.at(i * 2) * 2.5f;
			}
		}
//		qDebug() << "Time" << Timer.elapsed();
//		qDebug() << __FILE__ << __LINE__ << "tempBuf" << tempBuf[SAMPLINGSTRIPE / 2];
		runDRM();

		Io_Led = Qt::green;
	}
	if(fromFile)
	{
		sf_close(sfin);
		tmpSnd.clear();
		qDebug() << __FILE__ << __LINE__ << "sf_close(sfin)";
	}
	// do NOT close pSoundInInterface
//	pSoundInInterface->Close();
	m_stopAck = true;
//	qDebug() << __FILE__ << __LINE__;
}


void DRMReceiver::runDRM()
{
	DSPFLOAT temp;
	temp = WMERFAC;
	if(temp < 0) temp = 0;
	m_downSampleFilter->downSample4(tempBuf);
	vecrInpData.clear();
	for(int i = 0;i < SAMPLINGSTRIPE;i++)
	{
		vecrInpData.push_back(tempBuf[i]);
	}

	while(input_samples_buffer_request == 0)
	{
		qDebug() << __FILE__ << __LINE__  << "input_samples_buffer_request == 0";
		demodulatorPtr->demodulate(resamp_signal,0);
	}

	m_im = 0;
	m_im = m_dataInputPtr->getData(m_downSampleFilter->filteredDataPtr(),
											 resamp_signal,m_rRation);
	if(m_im == 0)
	{
		qDebug() << __FILE__ << __LINE__ << "m_im = 0";
		msleep(10);
		return;
	}
	demodulatorPtr->demodulate(resamp_signal,m_im);
//	qDebug() << __FILE__ << __LINE__;
	newSamp = true;
}



bool DRMReceiver::GetInputSpec(vector<_REAL>& vecrData)
{
	extern int specLen;
	if(vecrInpData.empty())
		return false;
	int				i;
	_REAL			rNormData;
	const int iLenInputVector = 2 * 2048;
	const int iLenDSInputVector = 2048;

	/* Init input and output vectors */
	vecrData.resize(specLen, (_REAL) 0.0);
//	vecrData.resize(LEN_SPEC, (_REAL) 0.0);
//	qDebug() << __FILE__ << __LINE__ << specLen; // vecrData.size();

	/* Lock resources */
	//		Lock();
	mutex.lock();

	rNormData = 0.2 * (_REAL)iLenInputVector * iLenInputVector * _MAXSHORT;

	/* Copy data from shift register */
	for (i = 0; i < iLenDSInputVector; i++)
	{
		fft.input()[i] = vecrInpData[i*2]  * vecrHammingWindow[i];
		//				qDebug() << "xx" << vecrInpData[i*2];
	}

	/* Get spectrum */
	fft.execute();

	/* Log power spectrum data */
//	for (i = 0; i < LEN_SPEC; i++)
		for (i = 0; i < specLen; i++)
	{
		vecrData[i] = sqrt(norm(fft.output()[i]) / rNormData) * 900.0;
		//				qDebug() << "rNormData" << rNormData;

	}

	/* Release resources */
	//		Unlock();
	mutex.unlock();
	return true;
}

//void DRMReceiver::GetVectorSpace(vector<_COMPLEX>& veccData)
//{
////	int iInputBlockSize = GetInputBlockSize();
//	for (int i = 0; i < iInputBlockSize; i++)
//		vecSigSpacBuf[i] = (*pvecInputData)[i].cSig;

//	/* Init output vectors */
//	veccData.resize(iN_mux);

//	/* Do copying of data only if vector is of non-zero length which means that
//		the module was already initialized */
//	if (iN_mux != 0)
//	{
//		/* Lock resources */
////		Lock();
//		mutex.lock();

//		/* Copy vectors */
//		for (int i = 0; i < iN_mux; i++)
//			veccData[i] = vecSigSpacBuf[i];

//		/* Release resources */
////		Unlock();
//		mutex.unlock();
//	}
//}

