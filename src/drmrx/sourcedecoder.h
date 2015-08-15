#ifndef SOURCEDECODER_H
#define SOURCEDECODER_H
//#include <QFTP>
#include <QObject>
#include <QList>
#define PACKETBUFFERLEN 512
#define NUMSERVICES 4
#define MAXTRANSPORTLISTS 5

#include <QList>
#include <QDateTime>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include "qsstvdefs.h"

// enum emscStatus {INVALID,VALID,ALREADYRECEIVED};

enum edataGroupType {GENDATA,CAMESS,GENCA,MOTHEAD,MOTDATA,MOTDATACA};


struct dataSegment
{
	dataSegment(int newSize)
	{
		crcOK=FALSE;
		recovered=FALSE;
		segmentNumber=-1;
		data.resize(newSize);
		data.fill(0XAA,newSize);
	}

	void setData(QByteArray ba,short int segNumber,bool crcok)
	{
		crcOK=crcok;
		data=ba;
		segmentNumber=segNumber;
	}

	void clearData()
	{
		data.clear();
	}

	bool crcOK;
	bool recovered;
	short int segmentNumber;
	bool hasData() {return (crcOK||recovered);}
	QByteArray data;
};

struct dataPacket
{
//	void log();
	QByteArray ba;
	bool extFlag;
	bool crcFlag;
	bool sessionFlag;
	bool userFlag;
	bool crcOK;
	edataGroupType dataGroupType;
	unsigned char continuityIndex;
	unsigned char repetitionIndex;
	unsigned short segmentNumber;
	bool lastSegment;
	unsigned short transportID;
	unsigned char userAccessField;
	unsigned short segmentSize;
	int offset;
	int lenght;
	unsigned int advance(int numBytes)
	{
		ba.remove(0,numBytes);
		return ba.count();
	}
	unsigned int chop(int numBytes)
	{
		ba.chop(numBytes);
		return ba.count();
	}

};


struct dataBlock
{
//	void log();
	bool firstFlag;
	bool lastFlag;
	short packetID;
	bool  PPI;
	unsigned char continuityIndex;
	bool crcOK;
	unsigned int length;
	QByteArray ba;
};



struct transportBlock
{
	transportBlock(unsigned short tId)
	{
//			  qDebug() << __FILE__ << __LINE__;
			 clear();
		transportID=tId;
	}

	void clear()
	{
		//	  qDebug() << __FILE__ << __LINE__;
		totalSegments=0;
		headerReceived=FALSE;
		segmentsReceived=0;
		bodySize=0;
		headerSize=0;
		alreadyReceived=FALSE;
//			 blockList.clear(); // castro
		lastSegmentReceived=FALSE;
		defaultSegmentSize=0;
	}
	bool isComplete()
	{
		if(!headerReceived) return FALSE;
		if(segmentsReceived<totalSegments) return FALSE;
		return TRUE;
	}

	int isAlmostComplete()
	{
		if(!headerReceived) return 0;
		if(totalSegments==0) return 0;
		return (segmentsReceived*100)/totalSegments;
	}

	void setAlreadyReceived(bool aRx)
	{
		int i;
		if(aRx)
		{
			alreadyReceived=TRUE;
			for(i=0;i<dataSegmentPtrList.count();i++)
			{
				dataSegmentPtrList.at(i)->clearData();
			}
		}
	}

	unsigned short transportID;
	unsigned int bodySize;
	unsigned int headerSize;
	unsigned short contentType;
	unsigned short contentSubtype;
	unsigned int defaultSegmentSize;
	QString fileName;
	QString newFileName;
	bool headerReceived;
	bool alreadyReceived;
	unsigned short segmentsReceived;
	bool lastSegmentReceived;
	int totalSegments;
	QString callsign;
	//  QList <short int> blockList;
	QVector<dataSegment *> dataSegmentPtrList;
	int robMode;
	int interLeaver;
	int mscMode; // qam
	int mpx;
	int spectrum;
	QByteArray baBSR;
	uint modeCode; //mode(A=0,B=1,E=2) BW(0=2.3,1=2.5) prot(High=0,LOW=1) QAM(4=0,16=1,64=2)
};


struct bsrBlock
{
	bsrBlock(transportBlock *tb)
	{
		tbPtr=tb;
	}
	transportBlock *tbPtr;
};


class sourceDecoder : public QObject
{
	Q_OBJECT
public:
	explicit sourceDecoder(QObject *parent = 0);
	//	sourceDecoder(QObject *parent = 0);
	void init();
	bool decode();
	//    bool hasStarted(){return started;}
//	uint rxTransportID;
	bool checkSaveImage(QByteArray ba, transportBlock *tbPtr);
	void saveImage(transportBlock *tbPtr);
	bool storeBSR(transportBlock *tb,bool compat);
	QList <bsrBlock> *getBSR();
	QByteArray getFix(){return FixBa;}
private:
	bool setupDataBlock(unsigned char *buffer,bool crcIsOK,int len);
	bool setupDataPacket(QByteArray ba);
	void addDataSegment();
	bool addHeaderSegment();
	void loadParams(transportBlock *tbPtr, unsigned char paramID, int len);
	void writeData(transportBlock *tbPtr);
	QList<short unsigned int> fixBlockList;
	//		QList<short unsigned int> drmBlockList;
	transportBlock *getTransporPtr(unsigned short tId,bool create);

	// now extern
	//		int bodyTotalSegments;
	//		int rxSegments;

	void removeTransporPtr(transportBlock * ptr);
	unsigned char packetBuffer[PACKETBUFFERLEN];
	dataBlock currentDataBlock;
	dataPacket currentDataPacket;

	QList<transportBlock *> transportBlockPtrList;

	transportBlock *lastTransportBlockPtr;
	QList <int> erasureList;
	QByteArray holdingBuffer;
	short int lastContinuityIndex;
	bool checkIt;
	QList <bsrBlock> bsrList;
	QByteArray FixBa;
	// now extern
	//		emscStatus msc_valid;
	//		int currentSegmentNumber;
	bool picSaved;
	bool gotBsr;
};
#endif // SOURCEDECODER_H
