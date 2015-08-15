/***************************************************************************
 *   Copyright (C) 2004 by Johan Maes                                      *
 *   on4qz@telenet.be                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ftp.h"
#include "qsstvglobal.h"
//#include <qmessagebox.h>
#include <qfiledialog.h>

#include <qapplication.h>
#include "configparams.h"

ftpInterface *ftpIntf;

ftpInterface::ftpInterface()
{
	ftp=NULL;
  sourceFn=NULL;
//  progress=new QProgressDialog ( tr("Uploading file..."),tr("Cancel"), 0, 0);
	init();

}

ftpInterface::~ftpInterface()
{
	destroy();
	if(ftp) delete ftp;

}

void ftpInterface::init()
{
		if(ftp)
			{
				destroy();
				delete ftp;
			}
    ftp = new QFtp( 0);
    connect( ftp, SIGNAL(commandStarted(int)),SLOT(ftp_commandStarted(int)) );
    connect( ftp, SIGNAL(commandFinished(int,bool)),SLOT(ftp_commandFinished(int,bool)) );
    connect( ftp, SIGNAL(done(bool)),SLOT(ftp_done(bool)) );
    connect( ftp, SIGNAL(stateChanged(int)),SLOT(ftp_stateChanged(int)) );
    connect( ftp, SIGNAL(listInfo(const QUrlInfo &)),SLOT(ftp_listInfo(const QUrlInfo &)) );
    connect( ftp, SIGNAL(rawCommandReply(int, const QString &)),SLOT(ftp_rawCommandReply(int, const QString &)) );
//    connect( ftp, SIGNAL(dataTransferProgress(qint64,qint64)),SLOT(setProgress(qint64,qint64)) );
//    connect( ftp, SIGNAL(commandFinished(int,bool)), progress, SLOT(reset()) );
//    connect( progress, SIGNAL(canceled()), SLOT(slotAbort()) );
}

void ftpInterface::destroy()
{
    if ( ftp->state() != QFtp::Unconnected )
			{
        addToLog("FTP destroy",LOGFTP);
				 ftp->close();
			}
}

bool ftpInterface::uploadFile(QString fileName,QString fixFilename,bool deleteSource)
{
		delSrc=deleteSource;
//    progress->show();
    aborting=FALSE;
    addToLog("FTP uploadFile",LOGFTP);
    if ( fileName.isNull() )  return FALSE;
    ftpDone=TRUE;
    if(isUnconnected())
      {
        ftpDone=FALSE;
        connectToHost();
      }

    while(!ftpDone)
     {
       if(aborting) return FALSE;
       qApp->processEvents();
     }

    sourceFn=new QFile(fileName);
    if ( !sourceFn->open( QIODevice::ReadOnly ) )
		 {
//		   QMessageBox::critical( 0, tr("Upload error"),
//                tr("Can't open file '%1' for reading.").arg(fileName) );
				delete sourceFn;
				sourceFn=NULL;
        return FALSE;
    }
    QFileInfo fi( fileName );
		QFileInfo fin(fixFilename);
    addToLog(QString("FTP bytes: %1").arg(sourceFn->size()),LOGFTP);

    ftpDone=FALSE;
    int id=ftp->put( sourceFn, fi.fileName(),QFtp::Binary);
    addToLog(QString("FTP put file id: %1").arg(id),LOGFTP);
    while(!ftpDone)
     {
       if(aborting) return FALSE;
       qApp->processEvents();
     }
		if(!fixFilename.isEmpty())
			{
			 ftp->rename(fi.fileName(),fin.fileName());
			}
     addToLog("FTP starting progress",LOGFTP);
    //progress->exec(); // ### takes a lot of time!!!
    return TRUE;
}


bool ftpInterface::downloadFile(QString sourceFileName,QString destinationFilename)
{
    delSrc=FALSE;
    aborting=FALSE;
    addToLog("FTP downloadFile",LOGFTP);
    QFile *destFn;
    destFn=new QFile(destinationFilename);
    if(!destFn->open(QIODevice::WriteOnly))
      {
        addToLog(QString("FTP unable to open destinationFilename %1").arg(destinationFilename),LOGFTP);
        return FALSE;
      }


    if (sourceFileName.isNull() )  return FALSE;
    ftpDone=TRUE;
    if(isUnconnected())
      {
        ftpDone=FALSE;
        connectToHost();
      }

    while(!ftpDone)
     {
       if(aborting) return FALSE;
       qApp->processEvents();
     }
    if(isUnconnected())
      {
        addToLog("FTP connection failed",LOGFTP);
        return FALSE;
      }

    ftpDone=FALSE;
    ftpCommandSuccess=FALSE;
    ftp->get( sourceFileName, destFn,QFtp::Binary);
    addToLog(QString("FTP get sourcefile %1 destination %2").arg(sourceFileName).arg(destFn->fileName()),LOGFTP);
    while(!ftpDone)
     {
       if(aborting) return FALSE;
       qApp->processEvents();
     }
    if(!ftpCommandSuccess) return FALSE;
     addToLog(QString("FTP file: %1 bytes: %2").arg(destinationFilename).arg(QFile(destinationFilename).size()),LOGFTP);

     return TRUE;
}


void ftpInterface::connectToHost()
{
    destroy();
    ftp->connectToHost(host,ftpPort);
    ftp->login( user, passwd );
    if(!directory.isEmpty()) changePath(directory);
}

// This slot is connected to the QComboBox::activated() signal of the
// remotePath.
void ftpInterface::changePath( const QString &newPath )
{
    ftp->cd( newPath );
}


/****************************************************************************
**
** Slots connected to signals of the QFtp class
**
*****************************************************************************/

void ftpInterface::ftp_commandStarted(int id)
{
   addToLog(QString("FTP commandStarted id:%1, %2").arg(id).arg(ftp->currentCommand()),LOGFTP);
 if ( ftp->currentCommand() == QFtp::List )
  {
  }
}

void ftpInterface::ftp_commandFinished(int id,bool err)
{
  QIODevice *p;

  addToLog(QString("FTP commandFinished id:%1, error:%2").arg(id).arg(err),LOGFTP);
  if(err)
    {
      addToLog(QString("FTP error:%1").arg(ftp->errorString()),LOGFTP);
      //        QMessageBox::critical( 0, tr("FTP Error"), ftp->errorString() );
    }
  else
    {
      ftpCommandSuccess=TRUE;
    }

  if (delSrc) sourceFn->remove();
  p=ftp->currentDevice();
  if(p)
    {
      delete ftp->currentDevice();
    }
}

void ftpInterface::ftp_done( bool error )
{
		
    if ( error )
      {
//        QMessageBox::critical( 0, tr("FTP Error"), ftp->errorString() );

        // If we are connected, but not logged in, it is not meaningful to stay
        // connected to the server since the error is a really fatal one (login
        // failed).
        destroy();
        ftpDone=TRUE;
        return;
      }
    addToLog("FTP done:  no error",LOGFTP);
    ftpDone=TRUE;
}

bool ftpInterface::isLoggedIn()
{
  return ftp->state() == QFtp::LoggedIn;
}

bool ftpInterface::isUnconnected()
{
  return ftp->state() == QFtp::Unconnected;
}

void ftpInterface::ftp_stateChanged( int state )
{
    switch ( (QFtp::State)state ) {
        case QFtp::Unconnected:
            addToLog("FTPss Unconnected",LOGFTP);
            break;
        case QFtp::HostLookup:
            addToLog("FTPss Host lookup",LOGFTP);
            break;
        case QFtp::Connecting:
            addToLog("FTPss Connecting",LOGFTP);
            break;
        case QFtp::Connected:
            addToLog("FTPss Connected",LOGFTP);
            break;
        case QFtp::LoggedIn:
            addToLog("FTPss Logged In",LOGFTP);
            break;
        case QFtp::Closing:
            addToLog("FTPss Closing",LOGFTP);
            break;
    default:
            addToLog(QString("FTPss uknown %1").arg(state),LOGFTP);
     break;
    }
}

void ftpInterface::ftp_listInfo( const QUrlInfo &)
{
 
}

void ftpInterface::ftp_rawCommandReply( int code, const QString &text )
{
    addToLog(QString("FTP Raw Command Reply: code=%1 , %2").arg(code).arg(text),LOGFTP);
}

//void ftpInterface::setProgress(qint64 txfrBytes,qint64 totalBytes)
//{
//  progress->setMaximum(totalBytes);
//  progress->setValue(txfrBytes);
//}

void ftpInterface::slotAbort()
{
  aborting=TRUE;
  ftp->abort();
}



