#include "drmtransmitter.h"
#include <QDebug>
#include "configparams.h"
#include <unistd.h>
#include <QSettings>

extern bool stopDRM;

extern CDRMTransmitter *DRMTransmitter;


// timing table;


int partTable[BWs][MODES][PROTECTIONS][QAMS]=
{
  {
    {{96,160,240},{67,140,201}},
    {{49,103,149},{96,160,240}},
    {{67,112,168},{49,83,124}}
  },
  {
    {{104,174,261},{78,163,235}},
    {{54,113,163},{104,174,261}},
    {{78,130,196},{54,90,135}}
  }
};


drmTransmitter::drmTransmitter()
{
//  DRMTransmitter=NULL;
  qDebug() << __FILE__ << __LINE__ << "************ HERE new drmTransmitter()";
}


drmTransmitter::~drmTransmitter()
{
	 qDebug() << __FILE__ << __LINE__ << "************ HERE ~drmTransmitter()";
//    sleep(1);
}


void drmTransmitter::init(QByteArray *ba, QString name, QString format, drmTxParams params)
{
//  int blocks;
	 int payLoad;
  dataLength=ba->count();


//  int picSize;
  ERobMode RobMod;
  ESpecOcc BW ;
  CParameter::ESymIntMod eNewDepth;
  ECodScheme eNewScheme;
  CMSCProtLev eNewMSCPre;
  CService Service;
  DRMTransmitter->init_base();
  DRMTransmitter->GetAudSrcEnc()->ClearPicFileNames();
  TransmParam = DRMTransmitter->GetParameters();
//  picSize=ba->size();
  DRMTransmitter->GetAudSrcEnc()->SetPicFileName(ba,name,format);
  switch (params.robMode)
    {
    case 0 :
      RobMod = RM_ROBUSTNESS_MODE_A;
	 break;
    case 1 :
      RobMod = RM_ROBUSTNESS_MODE_B ;
	 break ;
    default:
      RobMod = RM_ROBUSTNESS_MODE_E ;
	 break;
    }
  switch (params.bandwith)
    {
    case 0 :
      BW = SO_0;
	 break;
    default :
      BW = SO_1 ;
	 break ;
    }
  TransmParam->InitCellMapTable(RobMod , BW);

  switch (params.interleaver)
    {
    case 0 :
	  eNewDepth = CParameter::SI_SHORT;
	 break ;
    default :
	  eNewDepth = CParameter::SI_LONG;
	 break;
    }
  TransmParam->SetInterleaverDepth(eNewDepth);

  switch (params.qam)
    {
    case 0 :
      eNewScheme = CS_1_SM ; //4Bit QAM
	 break;
    case 1 :
      eNewScheme = CS_2_SM ; //16Bit QAM
	 break ;
    default :
      eNewScheme = CS_3_SM; // 64Bit QAM
	 break;
    }
  TransmParam->SetMSCCodingScheme(eNewScheme);
  switch (params.protection)
    {
    case 1 :
      eNewMSCPre.iPartB = 1;  //Norm
		 break;
    default:
      eNewMSCPre.iPartB = 0 ; //High
		 break ;
    }
  TransmParam->SetMSCProtLev(eNewMSCPre, FALSE);
  Service.iServiceDescr=0;
  Service.iServiceID=0;
  Service.iLanguage=5;
  Service.strLabel=params.callsign.toLatin1().data();
  TransmParam->SetServiceParameters(0,Service);
  DRMTransmitter->Init();
  // calculate transmision time
  payLoad=(TransmParam->iNumDecodedBitsMSC/SIZEOF__BYTE)-14;
  blocks=(dataLength+payLoad-1)/payLoad;
  duration=((double)(blocks+15+4))*0.400;
  transmissionTime = duration;
  qDebug()<< QString("datalen: %1 pl:%2,blocks:%3 time=%4").arg(dataLength).arg(payLoad).arg(blocks).arg(duration);
}

void drmTransmitter::start(bool startTx)
{

  if(startTx)
    {
      stopDRM=FALSE;
		DRMTransmitter->Start();
    }
  else
    {
      stopDRM=TRUE;
    }
}

drmTxParams modeToParams(uint mode)
{
  drmTxParams prm;
  prm.robMode=mode/10000;
  mode-=(mode/10000)*10000;
  prm.bandwith=mode/1000;
  mode-=(mode/1000)*1000;
  prm.protection=mode/100;
  mode-=(mode/100)*100;
  prm.qam=mode/10;
  prm.interleaver=0;
  prm.callsign=myCallsign;
  return prm;
}

uint paramsToMode(drmTxParams prm)
{
  uint mode=1;
  mode+=prm.robMode*10000;
  mode+=prm.bandwith*1000;
  mode+=prm.protection*100;
  mode+=prm.qam*10;
  return mode;
}



