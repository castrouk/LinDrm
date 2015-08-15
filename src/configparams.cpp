/**************************************************************************
*   Copyright (C) 2000-2012 by Johan Maes                                 *
*   on4qz@telenet.be                                                      *
*   http://users.telenet.be/on4qz                                         *
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
#include "configparams.h"

QString rxImagesPath; //!< path to the received images directory
QString txImagesPath; //!< path to the transmit images directory
QString templatesPath; //!< path to the templates directory
QString audioPath; //!< directory path to the audio files
QString docURL; //!< url of documentation
QString defaultImageFormat; //!< the default format for to save the image in (e.g. png, jpg ..)
//QString fontString;
//int fontSize;
int samplingrate; //!< samplingrate of the souncard (the nominal sampling frequency i.e. 8000, 11025 ...)
double rxClock;   //!< adjusted receive Clock
double txClock;	 //!< adjusted transmit Clock

QString serialPort;
QString pttSerialPort;
QString radioModel;
int radioModelNumber;
int civAddress;
int baudrate;
QString parity;
int stopbits;
int databits;
QString handshake;
bool enableCAT;
bool enableSerialPTT;
QString cwText; //!< message to send in CW after sending the image
int cwTone; //!< frequency of the CW signal
int cwWPM; //!< CW words per minute

bool enableFTP;
int ftpPort;
QString ftpRemoteHost;
QString ftpRemoteDirectory;
QString ftpLogin;
QString ftpPassword;
QString ftpFilename;
QString ftpDefaultImageFormat;

bool enableHybrid;
int hybridFtpPort;
QString hybridFtpRemoteHost;
QString hybridFtpRemoteDirectory;
QString hybridFtpLogin;
QString hybridFtpPassword;

int repeaterImageInterval;
bool repeaterEnable;
// esstvMode repeaterTxMode;
QString repeaterImage1;
QString repeaterImage2;
QString repeaterImage3;
QString repeaterImage4;
QString idleTemplate;
QString repeaterTemplate;
QString repeaterAcknowledge;



QString myCallsign;
QString myQth;
QString myLocator;
QString myLastname;
QString myFirstname;

QString startPicWF;
QString endPicWF;
QString fixWF;
QString bsrWF;




QString videoDevice;
//int framesPerSecondIndex;
int colorFormatIndex;
int sizeIndex;
int channelNumber;
// etransmissionMode transmissionModeIndex;  // SSTV or DRM or FAX
unsigned int dataScopeOffset;

