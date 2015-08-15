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
#ifndef FTPINTERFACE_H
#define FTPINTERFACE_H

#include "qglobal.h"

# if(QT_VERSION > QT_VERSION_CHECK(5, 0, 0))
#include "qftp.h"
# else
// #include <QFtp>
#endif

#include <QObject>
#include <QFile>
#include <QProgressDialog>

class ftpInterface: public QObject
{
	Q_OBJECT
	public:
	ftpInterface();
	~ftpInterface();
  void setupConnection(QString tHost,QString tUser,QString tPasswd,QString tDirectory)
  {
    host=tHost;
    user=tUser;
    passwd=tPasswd;
    directory=tDirectory;

  }
  bool uploadFile(QString fileName,QString fixFilename, bool deleteSource);
  bool downloadFile(QString sourceFileName,QString destinationFilename);
	bool isUnconnected();
  bool isLoggedIn();
private slots:
  void ftp_commandStarted(int);
  void ftp_commandFinished(int,bool);
	void ftp_done(bool);
	void ftp_stateChanged(int);
	void ftp_listInfo(const QUrlInfo &);
	void ftp_rawCommandReply(int, const QString &);
//  void setProgress(qint64,qint64);
  void slotAbort();
private:
	void connectToHost();
	void changePath( const QString &newPath );
	void destroy();
	void init();
    QFtp *ftp;
	QFile *sourceFn;
	bool delSrc;
//  QProgressDialog *progress;
  bool ftpDone;
  bool aborting;
  QString host;
  QString user;
  QString passwd;
  QString directory;
  bool ftpCommandSuccess;
};

extern ftpInterface *ftpIntf;

#endif
