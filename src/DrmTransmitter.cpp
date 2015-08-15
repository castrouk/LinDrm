/******************************************************************************\
 * Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
 * Copyright (c) 2001
 *
 * Author(s):
 *	Volker Fischer
 *
 * Adapted for ham sstv use by Ties Bos - PA0MBO
 *
 * Description:
 *	DRM-transmitter
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

#include "DrmTransmitter.h"
#include "sound/sound.h"
#include "soundcard.h"
#include <QSettings>
#include <QDebug>
#include "utils/supportfunctions.h"

extern bool stopDRM;

/* Implementation *************************************************************/
void CDRMTransmitter::Start()
{
	TransmParam.bRunThread = TRUE; // Set run flag
	Init(); // Initialization of the modules

	start();
}

void CDRMTransmitter::Stop()
{
	TransmParam.bRunThread = FALSE;
	stopDRM = true;
}

void CDRMTransmitter::Open_WF_file(QString fn)
{

	qDebug() << __FILE__ << __LINE__;
	QString sfname = fn;
//	QString sfname = "/home/castro/.config/G4EKQ/fix.xbm.flac";

	qDebug() << "sfname" << sfname;     /* Used to be called samples. */
	sfin = sf_open(sfname.toLatin1().data(), SFM_READ, &sfinfo);
	qDebug() << "frames" << sfinfo.frames;     /* Used to be called samples. */
	qDebug() << "samplerate" << sfinfo.samplerate;
	qDebug() << "channels" << sfinfo.channels;
	qDebug() << "format" << sfinfo.format;
	qDebug() << "sections" << sfinfo.sections;
	qDebug() << "seekable" << sfinfo.seekable;
	qDebug()<< __FILE__ << __LINE__;
}


bool CDRMTransmitter::Read_WF_file(CVector<_SAMPLE>& psData, bool loop = false)
{
	int len = BUFFERSIZE / 2;
	sf_count_t c = sf_readf_short(sfin, &psData[0], len);
	if(c != len)
	{
		if(loop)
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

void CDRMTransmitter::run()
{
	/*
  The hand over of data is done via an intermediate-buffer. The calling
  convention is always "input-buffer, output-buffer". Additional, the
  DRM-parameters are fed to the function
*/

//	QString cmd("alsactl --file ~/.config/G4EKQ/LinDrm/asound.state restore");
//	system(cmd.toLatin1().data());

	CVector<short> tmpSnd(BUFFERSIZE); // BUFFERSIZE 32768
//	qDebug() << __FILE__ << __LINE__ << "BUFFERSIZE" << BUFFERSIZE;
	qDebug() << __FILE__ << __LINE__ << "**************************************";
	Open_WF_file(m_wfFile);
	m_stopAck = false;
	wfCount = 0;
	while(!wf_done)
	{
		if(stopDRM)
			return;
		wf_done = Read_WF_file(tmpSnd);
		pSoundOutInterface->Write(tmpSnd);
		wfCount++;
		emit upDateWF(wfCount);
#if 0
		extern bool toSndFile;
		extern QVector<_SAMPLE> audio;
		if(toSndFile)
		{
			int len = tmpSnd.size() / 2;
			for(int i = 0; i < len; i++)
			{
				audio << tmpSnd.at(i * 2);
			}
		}
#endif
	}
	while (TransmParam.bRunThread)
	{
		AudioSourceEncoder.ProcessData(TransmParam,  AudSrcBuf);
//		qDebug() << __FILE__ << __LINE__ << "AudSrcBuf.GetFillLevel" << AudSrcBuf.GetFillLevel();

		//arrayBinDump(QString("audiosrc %1").arg(runCounter++),AudSrcBuf,32,TRUE);
		MSCMLCEncoder.ProcessData(TransmParam, AudSrcBuf, MLCEncBuf);
		SymbInterleaver.ProcessData(TransmParam, MLCEncBuf, IntlBuf);
		GenerateFACData.ReadData(TransmParam, GenFACDataBuf);
		FACMLCEncoder.ProcessData(TransmParam, GenFACDataBuf, FACMapBuf);
		OFDMCellMapping.ProcessData(TransmParam, IntlBuf, FACMapBuf, CarMapBuf);
		OFDMModulation.ProcessData(TransmParam, CarMapBuf, OFDMModBuf);
		TransmitData.WriteData(TransmParam, OFDMModBuf);
		//      arrayComplexDump(QString("cd "),OFDMModBuf.getVecBuffer(),8,TRUE);
//		usleep(5000);

		if (stopDRM)
		{
			TransmParam.bRunThread=FALSE;
		}
	}
	qDebug() << __FILE__ << __LINE__ << "stopDRM" << stopDRM;
//	pSoundOutInterface->Close();
	m_stopAck = true;
	emit EndTx();

//	stopDRM = true;
}

void CDRMTransmitter::Init()
{
	int PacLen, nr_decoded_bits  ;   // added pa0mbo

	OFDMCellMapping.Init(TransmParam, CarMapBuf); // Defines number of cells, important!
	//	SDCMLCEncoder.Init(TransmParam, SDCMapBuf); // Defines number of SDC bits per super-frame
	MSCMLCEncoder.Init(TransmParam, MLCEncBuf);
	nr_decoded_bits = TransmParam.iNumDecodedBitsMSC ;
	PacLen = (nr_decoded_bits/8) - 3 ;
	qDebug() << __FILE__ << __LINE__ << "PacLen" << PacLen;
	// printf("PacLen is %d\n", PacLen);
	TransmParam.Service[0].DataParam.iPacketLen=PacLen;

	// added Oct 19th 2011 pa0mbo
	TransmParam.iNumAudioService=0;
	TransmParam.iNumDataService =1 ;
	TransmParam.Service[0].eAudDataFlag = CService::SF_DATA;
	TransmParam.Service[0].DataParam.iStreamID = 0;
	TransmParam.Service[0].DataParam.eDataUnitInd = CDataParam::DU_DATA_UNITS;
	TransmParam.Service[0].DataParam.eAppDomain = CDataParam::AD_DAB_SPEC_APP;
	// end added block
	SymbInterleaver.Init(TransmParam, IntlBuf);
	GenerateFACData.Init(TransmParam, GenFACDataBuf);
	FACMLCEncoder.Init(TransmParam, FACMapBuf);
	OFDMModulation.Init(TransmParam, OFDMModBuf);
	AudioSourceEncoder.Init(TransmParam, AudSrcBuf);
	TransmitData.Init(TransmParam);
}

CDRMTransmitter::CDRMTransmitter() :
	pSoundOutInterface(new CSoundOut),
	TransmitData(pSoundOutInterface),
	// UEP only works with Dream receiver, FIXME! -> disabled for now
	bUseUEP(FALSE)
{
	qDebug() << "\n\n\n\n\n" << __FILE__ << __LINE__<< pSoundOutInterface;
	qDebug() << __FILE__ << __LINE__;
	QSettings settings("G4EKQ", "LinDRM");
	settings.beginGroup("DrmTX");
	devName = settings.value("SoundOUTdev").toString();
	pSoundOutInterface->SetDev(devName.toStdString());

	pSoundOutInterface->Init(48000, 38400,true);
	qDebug() << __FILE__ << __LINE__ << "devName" << pSoundOutInterface->GetDev().c_str();
	qDebug() << "\n\n\n\n\n";
//	int BufferSize = BUFFERSIZE;
	wf_done = false;
}

void CDRMTransmitter::init_base()
{
//	wf_done = false;

	TransmParam.init();
	/* Init streams */
	TransmParam.ResetServicesStreams();

	/* Init frame ID counter (index) */
	TransmParam.iFrameIDTransm = 0;

	/* Date, time. TODO: use computer system time... */
	TransmParam.iDay = 0;
	TransmParam.iMonth = 0;
	TransmParam.iYear = 0;
	TransmParam.iUTCHour = 0;
	TransmParam.iUTCMin = 0;


	/**************************************************************************/
	/* Robustness mode and spectrum occupancy. Available transmission modes:
	  RM_ROBUSTNESS_MODE_A: Gaussian channels, with minor fading,
	  RM_ROBUSTNESS_MODE_B: Time and frequency selective channels, with longer
	  delay spread,
	  RM_ROBUSTNESS_MODE_C: As robustness mode B, but with higher Doppler
	  spread,
	  RM_ROBUSTNESS_MODE_D: As robustness mode B, but with severe delay and
	  Doppler spread.
	  Available bandwidths:
	  SO_0: 4.5 kHz, SO_1: 5 kHz, SO_2: 9 kHz, SO_3: 10 kHz, SO_4: 18 kHz,
	  SO_5: 20 kHz

			  PA0MBO: for ham use now only modes A, B and E */
	TransmParam.InitCellMapTable(RM_ROBUSTNESS_MODE_E, SO_1);                            // was B pa0mbo 21-10-2011

	/* Protection levels for MSC. Depend on the modulation scheme. Look at
	  TableMLC.h, iCodRateCombMSC16SM, iCodRateCombMSC64SM,
	  iCodRateCombMSC64HMsym, iCodRateCombMSC64HMmix for available numbers */
	TransmParam.MSCPrLe.iPartA = 0;
	TransmParam.MSCPrLe.iPartB = 0;
	TransmParam.MSCPrLe.iHierarch = 0;

	/* Either one audio or one data service can be chosen */
	//  _BOOLEAN bIsAudio = FALSE;

	CService Service;

	/* In the current version only one service and one stream is supported. The
	  stream IDs must be 0 in both cases */


	/* Data Service only, no Audio*/
	TransmParam.SetNumOfServices(0,1);
	TransmParam.SetCurSelDataService(0);
	TransmParam.SetAudDataFlag(0,  CService::SF_DATA);
	CDataParam DataParam;
	DataParam.iStreamID = 0;
	/* Init SlideShow application */
	DataParam.iPacketLen = 45; /* TEST */
	DataParam.eDataUnitInd = CDataParam::DU_DATA_UNITS;
	DataParam.eAppDomain = CDataParam::AD_DAB_SPEC_APP;
	TransmParam.SetDataParam(0, DataParam);

	/* The value 0 indicates that the application details are provided solely by SDC data entity type 5 */
	Service.iServiceDescr = 0;


	/* Init service parameters, 24 bit unsigned integer number */
	Service.iServiceID = 0;

	// Service label data. Up to 16 bytes defining the label using UTF-8 coding
	Service.strLabel = "MYCALL";

	/* Language (see TableFAC.h, "strTableLanguageCode[]") */
	Service.iLanguage = 5; /* 5 -> english */

	TransmParam.SetServiceParameters(0, Service);

	/* Interleaver mode of MSC service. Long interleaving (2 s): SI_LONG, short interleaving (400 ms): SI_SHORT */
	TransmParam.eSymbolInterlMode = CParameter::SI_LONG;

	/* MSC modulation scheme. Available modes:
	  16-QAM standard mapping (SM): CS_2_SM,
	  64-QAM standard mapping (SM): CS_3_SM,
	  64-QAM symmetrical hierarchical mapping (HMsym): CS_3_HMSYM,
	  64-QAM mixture of the previous two mappings (HMmix): CS_3_HMMIX */
	TransmParam.eMSCCodingScheme = CS_2_SM;      // was CS_3_SM pa0mbo 21-11-2011

	/* SDC modulation scheme. Available modes:
	  4-QAM standard mapping (SM): CS_1_SM,
	  16-QAM standard mapping (SM): CS_2_SM */
	// 	TransmParam.eSDCCodingScheme = CS_2_SM;    pa0mbo

	/* Set desired intermedia frequency (IF) in Hertz */
	SetCarOffset(350.0); /* Default: "VIRTUAL_INTERMED_FREQ" was 12000.0  pa0mbo */
	rDefCarOffset=(_REAL) VIRTUAL_INTERMED_FREQ;

	if (bUseUEP == TRUE)
	{
		// TEST
		TransmParam.SetStreamLen(0, 80, 0);
	}
	else
	{
		/* Length of part B is set automatically (equal error protection (EEP),
		 if "= 0"). Sets the number of bytes, should not exceed total number
		 of bytes available in MSC block */
		TransmParam.SetStreamLen(0, 0, 0);
	}
}

