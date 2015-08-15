#include "sourcedecoder.h"
#include "drm.h"
// #include "qsstvglobal.h"
#include "drmproto.h"
#include <math.h>
#include <float.h>
#include "configparams.h"
// #include "dispatcher.h"
#include <QApplication>
#include <QFileInfo>
#include "utils/reedsolomoncoder.h"
#include <QFile>
#include <QDebug>
#include "drmrx/demodulator.h"
// #include "utils/ftp.h"
#include "configparams.h"
// #include "utils/qjp2io.h"
#include <QImage>
#include "bsrfix.h"

enum emscStatus {INVALID,VALID,ALREADYRECEIVED};

emscStatus msc_valid;


QString rxFname;
int bodyTotalSegments;
int rxSegments;
uint rxTransportID;
short sn = 1;


QList<short unsigned int> drmBlockList;

// sourceDecoder srcDecoder;

sourceDecoder::sourceDecoder(QObject *parent) : QObject(parent)
{

}

void sourceDecoder::init()
{
	lastTransportBlockPtr=NULL;
	transportBlockPtrList.clear();
	bodyTotalSegments=0;
	checkIt=FALSE;
	erasureList.clear();
	//	demodulatorPtr->init();
}


/*!
	\brief decode of valid data block

• header     8 bits.
• data field n bytes.
• CRC        16 bits.


	\return bool return TRUE if successful
*/
bool sourceDecoder::decode()
{
	double checksum;
	int N_partB;
	//  if(!demodulatorPtr->isTimeSync())

	if (channel_decoded_data_buffer_data_valid != 1)
	{
		//		qDebug() << "bool sourceDecoder::decode() -- if (channel_decoded_data_buffer_data_valid != 1)";
		return FALSE;
	}
	if (audio_data_flag == 0)
	{
		//		      addToLog("audio decoding not implemented in qsstv !\n",LOGDRMSRC);
		return FALSE;
	}
	//  addToLog("Datapacket received",LOGPERFORM);
	N_partB = (int) (length_decoded_data / 8);
	//  addToLog(QString("N-partB lenght=%1").arg(N_partB),LOGDRMSRC);
	//		 if(N_partB>PACKETBUFFERLEN)
	//		    {
	//		      addToLog(QString("packet buffer length exceeded: lenght=%1").arg(N_partB),LOGDRMMOT);
	//		    }
	bits2bytes (channel_decoded_data_buffer, N_partB * 8, packetBuffer);
	crc16_bytewise(&checksum, packetBuffer,N_partB);
	if(fabs (checksum) <= DBL_EPSILON)
	{
		if(!setupDataBlock(packetBuffer,TRUE,N_partB))
		{
//			qDebug() << __FILE__ << __LINE__;
			return FALSE;
		}
	}
	else
	{
		msc_valid=INVALID;
		//				MSC_Led = Qt::red;
		return FALSE;
	}
	// at this point we have a dataPacket we now check header / data and buils a transport stream
	switch(currentDataPacket.dataGroupType)
	{
	case MOTDATA:
		//						qDebug() << __FILE__ << __LINE__ <<  "addToLog(Datasegment,LOGDRMSRC);";
//		qDebug() << __FILE__ << __LINE__;
		addDataSegment();
		break;
	case MOTHEAD:
		//						qDebug() << __FILE__ << __LINE__ <<  "addToLog(Headersegment,LOGDRMSRC);";
//		qDebug() << __FILE__ << __LINE__;
		addHeaderSegment();
		break;
	default:
		return FALSE;
		break;
	}
	//				qDebug() << __FILE__ << __LINE__ <<  "return TRUE;";
	return TRUE;
}



bool sourceDecoder::setupDataBlock(unsigned char *buffer,bool crcIsOK,int len)
{
	//	qDebug() << __FILE__ << __LINE__ << "setupDataBlock";
	currentDataBlock.length=len;
	unsigned char header=buffer[0];
	unsigned int availableBytes;
	const char *bufPtr;
	currentDataBlock.ba=QByteArray((char *)buffer,len);
	currentDataBlock.firstFlag=currentDataBlock.lastFlag=FALSE;
	if(header & 0x80) currentDataBlock.firstFlag = TRUE;
	if(header & 0x40) currentDataBlock.lastFlag  = TRUE;
	currentDataBlock.packetID = (header & 0x30) >> 4;
	currentDataBlock.PPI = (header & 0x8) >> 3;
	currentDataBlock.continuityIndex = (header & 0x7);
	currentDataBlock.crcOK=crcIsOK;
	if ((currentDataBlock.PPI != 0) && (crcIsOK))
	{
		availableBytes=buffer[1];
		bufPtr=(const char *)&buffer[2];
	}
	else
	{
		availableBytes=len-3;
		bufPtr=(const char *)&buffer[1];
	}
	if(currentDataBlock.firstFlag)
	{
		holdingBuffer.clear();
	}
	holdingBuffer.append(bufPtr,availableBytes);
	if(currentDataBlock.lastFlag)
	{
		return setupDataPacket(holdingBuffer);
	}
	return FALSE;
}


//MSC Header Description 2 bytes or 4 bytes if Extension field
// First Byte
// B7 Extension Flag    Header has 2 more bytes in the Extension field
// B6 CRC Flag          MOT has CRC
// B5 Session Flag      MOT last flag and segment number present.
// B4 UserAccess Flag   Mot user access field present.
// B3-B0 Data Group Type
//    0 0 0 0  (0) General data;
//    0 0 0 1  (1) CA messages (for example ECMs or EMMs: see subclause 9.3.2.1);
//    0 0 1 0  (2) General data and CA parameters (for example, DGCA);
//    0 0 1 1  (3) MOT header information;
//    0 1 0 0  (4) MOT data
//    0 1 0 1  (5) MOT data and CA parameters.

//Second Byte
// B7-B4 Continuity Index
// B3-B0 Repetition Index

// 2 more extension bytes if flag is set
// B15-B0 Extension Field - no further specifications

// Session Header (presence indicated by Session Flag

// if last flag and segment number present
// B15    last segment
// B14-B0 segment number

// user access fields
// B7-B5  rfa (not used)
// B4     TransportID present flag
// B3-B0  length of user access fields

// if set
// B15-B0  TransportID
// other bytes filled with End user address field

// This is followed with a 2 bytes Segmentation header
// B15-B13 Repetition Count (not used)
// B12-B0  Length of the data that follows

int currentSegmentNumber;

bool sourceDecoder::setupDataPacket(QByteArray ba)
{
	double checksum;
	unsigned char lengthIndicator;
	unsigned char header;

	currentDataPacket.ba=ba; // drop crc
	header=ba.at(0);
	currentDataPacket.transportID = 0xFFFF;
	currentDataPacket.extFlag=FALSE;
	currentDataPacket.crcFlag=FALSE;
	currentDataPacket.sessionFlag=FALSE;
	currentDataPacket.userFlag=FALSE;
	currentDataPacket.lastSegment=FALSE;
	currentDataPacket.crcOK=currentDataBlock.crcOK;

	if(header&0x10) currentDataPacket.userFlag=TRUE;
	currentDataPacket.dataGroupType=(edataGroupType)(header&0x07);
	if(header&0x80) currentDataPacket.extFlag=TRUE;
	if(header&0x20) currentDataPacket.sessionFlag=TRUE;

	if(header&0x40)
	{
		currentDataPacket.crcFlag=TRUE;
		crc16_bytewise (&checksum,(unsigned char *)currentDataPacket.ba.data(),currentDataPacket.ba.count());
		if (fabs (checksum) <= DBL_EPSILON)
		{
			currentDataPacket.crcOK=TRUE;
		}
		else
		{
			currentDataPacket.crcOK=FALSE;
			msc_valid=INVALID;
			//						MSC_Led = Qt::red;
			return FALSE;
		}
		currentDataPacket.chop(2); // drop crc
	}
	currentDataPacket.advance(2); //skip header and continuity bits
	if(currentDataPacket.extFlag) currentDataPacket.advance(2); // just skip the extension bytes

	if(currentDataPacket.sessionFlag)
	{
		currentDataPacket.segmentNumber = (((unsigned char)(currentDataPacket.ba.at(0)) & 0x7F))*256+ ((unsigned char)currentDataPacket.ba.at(1))  ;
		if(currentDataPacket.ba.at(0)&0x80)
		{
			currentDataPacket.lastSegment=TRUE;
		}
		currentSegmentNumber=currentDataPacket.segmentNumber;
		currentDataPacket.advance(2);
	}

	if (currentDataPacket.userFlag)
	{
		currentDataPacket.userAccessField = (unsigned char)(currentDataPacket.ba.at(0));
		currentDataPacket.advance(1);
		lengthIndicator = (currentDataPacket.userAccessField& 0xF);

		if((currentDataPacket.userAccessField & 0x10) && (lengthIndicator>=2))
		{
			currentDataPacket.transportID = (((unsigned char)(currentDataPacket.ba.at(0)))) * 256 + ((unsigned char)currentDataPacket.ba.at(1));
		}
		currentDataPacket.advance(lengthIndicator);
	}
	currentDataPacket.segmentSize=(((unsigned char)(currentDataPacket.ba.at(0)) & 0x1F))*256+ ((unsigned char)currentDataPacket.ba.at(1));
	currentDataPacket.advance(2);
	currentDataPacket.lenght=currentDataPacket.ba.count();
	return TRUE;
}




bool sourceDecoder::addHeaderSegment()
{
	transportBlock *tbPtr;
	//		  addToLog(QString("Header segsize: %1").arg(currentDataPacket.segmentSize),LOGDRMSRC);
	tbPtr = getTransporPtr(currentDataPacket.transportID,TRUE);
	//	qDebug() << __FILE__ << __LINE__ << "addHeaderSegment()";
	if(!tbPtr->alreadyReceived) msc_valid=VALID;
	else
	{
		qDebug() << __FILE__ << __LINE__ << "(tbPtr->alreadyReceived)";
		msc_valid=ALREADYRECEIVED;
		//		tbPtr->setAlreadyReceived(false);
		return TRUE;
	}
	tbPtr->headerReceived=TRUE;
	unsigned char *dataPtr=(unsigned char *)currentDataPacket.ba.data();
	unsigned char PLI;
	unsigned char paramID;
	unsigned char extBit;
	unsigned short dataFieldLength;
	tbPtr->bodySize =
			(((unsigned int)dataPtr[0]) << 20) +
			(((unsigned int)dataPtr[1]) << 12) +
			(((unsigned int)dataPtr[2]) << 4) +
			((((unsigned int)dataPtr[3]) & 0xF0) >> 4);

	tbPtr->headerSize =
			(((unsigned int)dataPtr[3] & 0x0F) << 9) +
			(((unsigned int)dataPtr[4]) << 1) +
			((((unsigned int)dataPtr[5]) & 0x80) >> 7);

	tbPtr->contentType = (((unsigned int)dataPtr[5] & 0x7E) >> 1);
	tbPtr->contentSubtype = ((((unsigned int)dataPtr[5]) & 0x1) <<8) +((unsigned int)dataPtr[6]);
	currentDataPacket.advance(7); // size of header core
	// The header core is followed by a number of parameter blocks
	// the first byte of every parameter block contains a 2-bits PLI (B7 and B6) indicating the type of parameter block.

	while(currentDataPacket.ba.count())  // todo
	{
		PLI=dataPtr[0]>>6;
		paramID=dataPtr[0]&0x3F;
		switch (PLI)
		{
		case 0:
			currentDataPacket.advance(1);
			break;
		case 1:
			loadParams(tbPtr,paramID,1);
			currentDataPacket.advance(2);
			break;
		case 2:
			loadParams(tbPtr,paramID,4);
			currentDataPacket.advance(5);
			break;
		case 3:
			extBit=dataPtr[0]&0x80;
			if(extBit)
			{
				dataFieldLength=256*(dataPtr[0]&0x7F)+dataPtr[1];
				currentDataPacket.advance(2);
			}
			else
			{
				dataFieldLength=dataPtr[0]&0x7F;
				currentDataPacket.advance(1);
			}
			loadParams(tbPtr,paramID,dataFieldLength);
			currentDataPacket.advance(dataFieldLength);
			break;
		}
	}
	return TRUE;

}


void sourceDecoder::loadParams(transportBlock *tbPtr,unsigned char paramID,int len)
{
	// rxDRMStatusEvent *stce;
	QString tmp,t;
	int i;
	switch(paramID)
	{
	case 5: // expiration
		break;
	case 6:
		break;
	case 12:
		tbPtr->fileName=QString(currentDataPacket.ba.data()+1).left(len-1);
		rxFname = tbPtr->fileName;
		break;
	default:
		for(i=0;i<len;i++)
		{
			t=QString::number((unsigned char)currentDataPacket.ba.data()[i],16);
			if(t.length()<2) t.prepend("0");
			tmp+=t;
		}
		//				qDebug() <<"unresolved paramId"<<paramID << tmp;
		break;
	}
}


void sourceDecoder::addDataSegment()
{
	static uint tpid = 0;
	int i;
	transportBlock *tbPtr;
	tbPtr = getTransporPtr(currentDataPacket.transportID,TRUE);
	rxTransportID = currentDataPacket.transportID;
	if(tpid != rxTransportID)
	{
		tpid = rxTransportID;
		picSaved = false;
		gotBsr = false;
	}

	if(callsignValid)
	{
		tbPtr->callsign=drmCallsign;
	}
//	if((tbPtr->totalSegments > 0) || (tbPtr->totalSegments < 10000))
		bodyTotalSegments = tbPtr->totalSegments;
	if(!tbPtr->alreadyReceived)
		msc_valid=VALID;
	else
	{
		qDebug() << __FILE__ << __LINE__ << "msc_valid=ALREADYRECEIVED";
		msc_valid=ALREADYRECEIVED;
	}
	if(currentDataPacket.lastSegment)
	{
		tbPtr->totalSegments=currentDataPacket.segmentNumber+1;
		tbPtr->lastSegmentReceived=TRUE;
	}
	else
	{
		tbPtr->defaultSegmentSize=currentDataPacket.segmentSize;
	}
	if(tbPtr->defaultSegmentSize==0)
	{
		//      qDebug() << "def seg size not set";
	}
	for(i=tbPtr->dataSegmentPtrList.count();i<=currentDataPacket.segmentNumber;i++)
	{
		tbPtr->dataSegmentPtrList.append(new dataSegment(tbPtr->defaultSegmentSize));
	}

	if(!tbPtr->dataSegmentPtrList.at(currentDataPacket.segmentNumber)->hasData())
	{
		checkIt=TRUE;
	}
	else
	{
		msc_valid=ALREADYRECEIVED;
	}
	if(tbPtr->alreadyReceived)
	{
//		qDebug() << __FILE__ << __LINE__ << "(tbPtr->alreadyReceived)";
		msc_valid=ALREADYRECEIVED;
		checkIt=FALSE;
	}


	if(tbPtr->totalSegments < currentDataPacket.segmentNumber+1)
		tbPtr->totalSegments = currentDataPacket.segmentNumber+1;
	rxSegments = tbPtr->segmentsReceived;
	//  bytesReceived=rxSegments*tbPtr->defaultSegmentSize;

	tbPtr->dataSegmentPtrList.at(currentDataPacket.segmentNumber)->setData(currentDataPacket.ba,currentDataPacket.segmentNumber,TRUE);

	writeData(tbPtr);

}

void sourceDecoder::writeData(transportBlock *tbPtr)
{
	int i;

	QByteArray ba;

	int length=0;
	erasureList.clear();
	erasureList.append(tbPtr->totalSegments);
	erasureList.append(tbPtr->defaultSegmentSize);
	for(i=0;i<tbPtr->dataSegmentPtrList.count();i++)
	{
		if(!tbPtr->dataSegmentPtrList.at(i)->hasData())
		{
			erasureList.append(i);
			ba.append(QByteArray(tbPtr->defaultSegmentSize,0x00));
		}
		else
		{
			ba.append(tbPtr->dataSegmentPtrList.at(i)->data);
		}
		length+=tbPtr->dataSegmentPtrList.at(i)->data.size();
		// qDebug()<< "block" << i<< "len" << tbPtr->dataSegmentPtrList.at(i)->data.size() ;
	}
	tbPtr->segmentsReceived = 0;
	drmBlockList.clear();
	//	qDebug() << __FILE__ << __LINE__ << "tbPtr->segmentsReceived" << tbPtr->segmentsReceived;

	for(i = 0; i < tbPtr->dataSegmentPtrList.count(); i++)
	{
		if(tbPtr->dataSegmentPtrList.at(i)->hasData())
		{
			drmBlockList.append(i);
			tbPtr->segmentsReceived++;
		}
	}
	//	qDebug() << __FILE__ << __LINE__ << "tbPtr->segmentsReceived" << tbPtr->segmentsReceived;
	if(tbPtr->isAlmostComplete()< 63)
		return;
	if(!tbPtr->lastSegmentReceived)
		return;

	//	 qDebug() << __FILE__ << __LINE__ << "tbPtr->lastSegmentReceived";
	checkSaveImage(ba,tbPtr);
}





void sourceDecoder::saveImage(transportBlock *tbPtr)
{
	QImage test;
	//		displayTextEvent *stce;
	QString t;
	//		ftpInterface ftpIntf;
	//		bool done=FALSE;
	bool textMode=FALSE;
	QString downloadF;

	if(picSaved) return;
//	if(tbPtr->alreadyReceived) return ;
	if(tbPtr->fileName.isEmpty()) return ;

	if(tbPtr->totalSegments < 10)
	{
//		qDebug() << __FILE__ << __LINE__ << tbPtr->fileName.left(4);
		return;
	}
	if(tbPtr->fileName.left(4)==".de ")
		if(tbPtr->fileName.left(3)=="de_")
	{
//		qDebug() << __FILE__ << __LINE__ << "======== TO DO ( FTP ) maybe! ========";
		//		downloadF=tbPtr->fileName;
		//		downloadF.remove(0,4);
		//		downloadF=rxImagesPath+"/"+downloadF;
		//		ftpIntf.setupConnection(hybridFtpRemoteHost,hybridFtpLogin,hybridFtpPassword,hybridFtpRemoteDirectory);
		//		if(!ftpIntf.downloadFile(tbPtr->fileName,downloadF))
		//		{
		return;
		//		}
		//		tbPtr->newFileName=downloadF;
	}
	if(tbPtr->newFileName.isEmpty()) return ;
	//  test=readJP2Image(tbPtr->newFileName);
	if(!test.load(tbPtr->newFileName))
	{
		//						test=readJP2Image(tbPtr->newFileName);
		if(test.isNull())
		{
			// maybe text
			QFileInfo finfo(tbPtr->newFileName);
			if(finfo.suffix()=="txt")
			{
				qDebug() << __FILE__ << __LINE__ << "======== TO DO ( TEXT ) ========";
				QFile fi(tbPtr->newFileName);
				if(!fi.open(QIODevice::ReadOnly)) return;
				t=fi.readAll();
				//													stce= new displayTextEvent(t);
				//													QApplication::postEvent( dispatcherPtr, stce );  // Qt will delete it when done
				textMode=TRUE;
			}
		}
	}

//	tbPtr->setAlreadyReceived(TRUE);
	//	qDebug() << __FILE__ << __LINE__ << "// tbPtr->setAlreadyReceived(TRUE);";
	if(!textMode)
	{
		//		qDebug() << __FILE__ << __LINE__ << "======== tbPtr->newFileName:" << tbPtr->newFileName;
		QString cmd;
		cmd.sprintf("%s \"%s\" &","viewer",tbPtr->newFileName.toLatin1().data());
		system(cmd.toLatin1().data());
		checkIt=FALSE;
	}
}

bool sourceDecoder::checkSaveImage(QByteArray ba,transportBlock *tbPtr)
{
//	prepareFixEvent *pe;
	QByteArray fixba;
	QFile outFile;
	reedSolomonCoder rsd;
	QString fileName;
	QString extension;

	//				qWarning("TODO: %s	%d",__FILE__,__LINE__);
	rxImagesPath = "/home/castro/Work-64/LinDrm/RxPicNew";
	fileName = rxImagesPath+"/"+tbPtr->fileName;

	// 				qDebug() << __FILE__<< __LINE__ << "fileName" << fileName;
	tbPtr->newFileName=fileName;
	QByteArray baFile;
	QByteArray *baFilePtr;

	if(!checkIt) return FALSE;
	QFileInfo qfinf(fileName);
	extension=qfinf.suffix().toLower();
	if((extension=="rs1") || (extension=="rs2") ||(extension=="rs3")||(extension=="rs4"))
	{
		// try to decode
		if(tbPtr->alreadyReceived) return FALSE;
		if(!rsd.decode(ba,fileName,tbPtr->newFileName,baFile,extension,erasureList))
			return FALSE;
		baFilePtr=&baFile;
	}
	else
	{
		if(!tbPtr->isComplete())
			return FALSE;
		if(gotBsr)
			return true;
		tbPtr->newFileName=fileName;
		if((tbPtr->fileName == "bsr.bin")&&(!tbPtr->alreadyReceived))
		{
			gotBsr = true;
			qWarning("TODO: %s	%d",__FILE__,__LINE__);
			tbPtr->setAlreadyReceived(TRUE);
			FixBa = ba;
			bsrFix bfx;
			bfx.prepareFIX(ba);
//			pe = new prepareFixEvent(ba);
			//														QApplication::postEvent(dispatcherPtr, pe);
			return FALSE;
		}
		baFilePtr=&ba;
	}
//	if(!tbPtr->alreadyReceived)
	if(!picSaved)
	{
		outFile.setFileName(tbPtr->newFileName);
		if(outFile.open(QIODevice::WriteOnly) <= 0)
		{
			outFile.close();
			return FALSE;
		}
		outFile.write(*baFilePtr);
		outFile.close();
		erasureList.clear();
		saveImage(tbPtr);
		picSaved = true;
		sn++;
	}
	return FALSE;
}

QList <bsrBlock> *sourceDecoder::getBSR()
{
	int i;
	transportBlock *tbPtr;
	bsrList.clear();
	qDebug() << __FILE__ << __LINE__ << "getBSR";

	for(i=0;i<transportBlockPtrList.count();i++)
	{
		tbPtr=transportBlockPtrList.at(i);
		if(tbPtr->alreadyReceived) continue;
		if(tbPtr->fileName=="bsr.bin") continue;
		bsrList.append(bsrBlock(tbPtr));
	}
	return &bsrList;
}

bool sourceDecoder::storeBSR(transportBlock *tb, bool compat)
{
	int i;
	int needsFiller;
	int prevErasure=0;
	// QByteArray ba;
	qDebug() << __FILE__ << __LINE__ << "storeBSR";
	erasureList.clear();
	erasureList.append(tb->totalSegments);
	erasureList.append(tb->defaultSegmentSize);
	for(i=0;i<tb->dataSegmentPtrList.count();i++)
	{
		if(!tb->dataSegmentPtrList.at(i)->hasData())
		{
			erasureList.append(i);
			//         ba.append(QByteArray(tb->defaultSegmentSize,0x00));
		}
		else
		{
			//          ba.append(tb->dataSegmentPtrList.at(i)->data);
		}
		//      length+=tb->dataSegmentPtrList.at(i)->data.size();
		//      qDebug()<< "block" << i<< "len" << tbPtr->dataSegmentPtrList.at(i)->data.size() ;
	}
	tb->baBSR.clear();
	if(erasureList.count()<3) return FALSE; //erasurelist has already totalSegments and defaultSegmentSize
	tb->baBSR.append(QString::number(tb->transportID).toLatin1().data());
	tb->baBSR.append("\n");
	tb->baBSR.append("H_OK\n");
	tb->baBSR.append(QString::number(erasureList.at(1)).toLatin1().data());
	tb->baBSR.append("\n");
	tb->baBSR.append(QString::number(erasureList.at(2)).toLatin1().data());
	tb->baBSR.append("\n");

	prevErasure=erasureList.at(2);
	needsFiller=FALSE;
	for(i=3;i<erasureList.count();i++) //skip
	{
		if(((prevErasure+1)==erasureList.at(i))&&(compat))
		{
			needsFiller=TRUE;
		}
		else
		{
			if(needsFiller)
			{
				tb->baBSR.append(QString::number(-1).toLatin1().data());
				tb->baBSR.append("\n");
				needsFiller=FALSE;
			}
			tb->baBSR.append(QString::number(erasureList.at(i)).toLatin1().data());
			tb->baBSR.append("\n");
		}
		prevErasure=erasureList.at(i);
	}
	if(needsFiller)
	{
		tb->baBSR.append(QString::number(-1).toLatin1().data());
		tb->baBSR.append("\n");
		needsFiller=FALSE;
		tb->baBSR.append(QString::number(erasureList.at(erasureList.count()-1)).toLatin1().data());
		tb->baBSR.append("\n");
	}
	tb->baBSR.append("-99\n");
	if(!compat)
	{
		QString temp;
		tb->baBSR.append(tb->fileName+"\n");
		temp=QString::number(tb->modeCode);
		while(temp.length()<5) temp.prepend("0");
		tb->baBSR.append(temp);
	}
	return TRUE;
}


transportBlock *sourceDecoder::getTransporPtr(unsigned short tId,bool create)
{
	//		qWarning("TODO: %s	%d",__FILE__,__LINE__);
	int i;
	//					rxDRMStatusEvent *stce;
	bool found = FALSE;
	for(i=0; i < transportBlockPtrList.count(); i++)
	{
		if(transportBlockPtrList.at(i)->transportID == tId)
		{
			found=TRUE;
			break;
		}
	}
	if(found)
	{
		lastTransportBlockPtr = transportBlockPtrList.at(i);
	}
	else if (create)
	{
//		lastTransportBlockPtr->clear();
		//		qDebug() << __FILE__ << __LINE__ << "create";
		//								stce= new rxDRMStatusEvent("");
		//								QApplication::postEvent( dispatcherPtr, stce );  // Qt will delete it when done
		if(transportBlockPtrList.count() >= MAXTRANSPORTLISTS)
		{
			//delete the oldest
			transportBlockPtrList.removeFirst();
		}
		for(i=0;i<transportBlockPtrList.count();)
		{
			if(transportBlockPtrList.at(i)->fileName=="bsr.bin")
			{
				transportBlockPtrList.takeAt(i);
				//              qDebug()<< "deleting bsr.bin";
			}
			else i++;
		}


		transportBlockPtrList.append(new transportBlock(tId));
		lastTransportBlockPtr=transportBlockPtrList.last();
		lastTransportBlockPtr->robMode=robustness_mode;
		lastTransportBlockPtr->interLeaver=interleaver_depth_new;
		lastTransportBlockPtr->mscMode=msc_mode_new; // qam
		lastTransportBlockPtr->mpx=multiplex_description.PL_PartB;
		lastTransportBlockPtr->spectrum=spectrum_occupancy_new;


		// remap msc_new to modeCode
		int mCode=1;    //default QAM16
		if(msc_mode_new==3) mCode=0;
		if(msc_mode_new==0) mCode=2;
		int protection=0;
		if(multiplex_description.PL_PartB==1) protection=1;
		lastTransportBlockPtr->modeCode=robustness_mode*10000+spectrum_occupancy_new*1000+protection*100+mCode*10+1;
		/////////////// new ////////////
		//		qDebug() << __FILE__ << __LINE__;
		//		lastTransportBlockPtr->clear();
	}
	else
	{
		//		qDebug() << __FILE__ << __LINE__;
		return NULL;
	}
	//	qDebug() << __FILE__ << __LINE__;
	return lastTransportBlockPtr;
}

void sourceDecoder::removeTransporPtr(transportBlock * ptr)
{
	int i;
	for(i=0;i<transportBlockPtrList.count();i++)
	{
		if(transportBlockPtrList.at(i)==ptr)
		{
			transportBlockPtrList.takeAt(i);
			break;
		}
	}
}






