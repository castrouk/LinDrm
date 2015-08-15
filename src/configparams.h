#ifndef CONFIGPARAMS_H
#define CONFIGPARAMS_H
// #include "sstv/sstvparam.h"
#include <QString>


extern QString rxImagesPath;
extern QString txImagesPath;
extern QString templatesPath;
extern QString audioPath;
extern QString docURL;
extern QString defaultImageFormat;
//extern QString fontString;
//extern int fontSize;
extern int samplingrate;
extern double rxClock;
extern double txClock;
extern QString serialPort; /**<  serial port device*/
extern QString radioModel;
extern int radioModelNumber;
extern int civAddress;
extern int baudrate; /**<  serial port baudrate*/
extern QString parity;
extern int stopbits;
extern int databits;
extern QString handshake;
extern bool enableCAT;
extern bool enableSerialPTT;
extern QString pttSerialPort;


extern QString myCallsign;
extern QString myQth;
extern QString myLocator;
extern QString myLastname;
extern QString myFirstname;

extern QString cwText;
extern int cwTone;
extern int cwWPM;


extern bool enableFTP;
extern int ftpPort;
extern QString ftpRemoteHost;
extern QString ftpRemoteDirectory;
extern QString ftpLogin;
extern QString ftpPassword;
extern QString ftpFilename;
extern QString ftpDefaultImageFormat;

extern bool enableHybrid;
extern int hybridFtpPort;
extern QString hybridFtpRemoteHost;
extern QString hybridFtpRemoteDirectory;
extern QString hybridFtpLogin;
extern QString hybridFtpPassword;


extern bool repeaterEnable;
extern int repeaterImageInterval;
// extern esstvMode repeaterTxMode;
extern QString repeaterImage1;
extern QString repeaterImage2;
extern QString repeaterImage3;
extern QString repeaterImage4;
extern QString repeaterAcknowledge;
extern QString repeaterTemplate;
extern QString idleTemplate;

extern QString startPicWF;
extern QString endPicWF;
extern QString fixWF;
extern QString bsrWF;

extern QString videoDevice;
//extern int framesPerSecondIndex;
extern int colorFormatIndex;
extern int sizeIndex;
extern int channelNumber;
// extern etransmissionMode transmissionModeIndex;  // SSTV , DRM or FAX
extern unsigned int dataScopeOffset;
#endif // CONFIGPARAMS_H
