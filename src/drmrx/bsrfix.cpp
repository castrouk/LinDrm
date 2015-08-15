#include "bsrfix.h"
#include <QPushButton>

extern QList<short unsigned int> fixBlockList;
QList <txSession> txList;

txSession *FixSessionPtr;
txSession FixSession;


bsrFix::bsrFix()
{

}

bsrFix::~bsrFix()
{

}

bool bsrFix::prepareFIX(QByteArray bsrByteArray)
{
  int i,j;
//  unsigned int mode=0;
//  eRSType rsType;Ptr
  bool inSeries;
  QString fileName,extension;
  bool extended,done;
  int block;
  fixBlockList.clear();
  unsigned short trID,lastBlock;
  QString str(bsrByteArray);
  str.replace("\r","");
  //  information is in the QByteArray ba
  QStringList sl;
  sl=str.split("\n",QString::SkipEmptyParts);

  if(sl.at(1)!="H_OK")
	 {
		return FALSE;
	 }
  trID=sl.at(0).toUInt();
  qDebug() << __FILE__ << __LINE__ << "trID" << trID;
  lastBlock=sl.at(3).toUInt();
  fixBlockList.append(lastBlock++);
  inSeries=FALSE;
  done=FALSE;
  extended=FALSE;
  for(i=4;(!done)&&i<sl.count();i++)
	 {
		block=sl.at(i).toInt();
		if(block==-99)
		  {
			 done=TRUE;
			 i++;
			 break;
		  }
		if(block<0) inSeries=TRUE;
		else
		  {
			 if(inSeries)
				{
				  inSeries=FALSE;
				  for(j=lastBlock;j<block;j++) fixBlockList.append(j);
				}
			 fixBlockList.append(block);
			 lastBlock=block+1;
		  }
	 }
  // check if we have a filename beyond -99
  if((i+1)<sl.count()) // we need an additional 2 entries (filename and mode)
	 {
		extended=TRUE;
		fileName=sl.at(i++);
//      mode=sl.at(i).toUInt();
//      qDebug() << " bsr received with " << fileName << mode;
	 }
//  fixForm fx(mainWindowPtr);
//  sessionPtr=txFunctionsPtr->getSessionPtr(trID);
//  FixSessionPtr
//  extern QPushButton *FixPtr;
  bool res;
  res = getSessionPtr(trID);
  if ((!res) && (!extended))
	 {
//	  FixPtr->setEnabled(false);
		return FALSE;
	 }
  else if (res)
	 {
//		FixPtr->setEnabled(true);
		// take it from the transmitlist
		return FALSE;
		//        }
	 }
  return TRUE;
}

//txSession *bsrFix::getSessionPtr(uint transportID)
//{
//  int i;
//  for(i=0;i<txList.count();i++)
//	 {
//		if(txList.at(i).transportID==transportID)
//		  {
//			 return &txList[i];
//		  }
//	 }
//  return NULL;
//}


bool bsrFix::getSessionPtr(uint transportID)
{
  int i;
  for(i=0;i<txList.count();i++)
	 {
		if(txList.at(i).transportID==transportID)
		  {
			// return &txList[i];
			 FixSession = txList[i];
			 return true;
		  }
	 }
  return false;
}

