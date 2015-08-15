#ifndef DRMTRANSMITTER_H
#define DRMTRANSMITTER_H

#include "GlobalDefinitions.h"
#include "DrmTransmitter.h"


struct drmTxParams
{
  int robMode;
  int qam;
  int bandwith;
  int interleaver;
  int protection;
  QString callsign;
  int readSolomon;
};


class drmTransmitter
{
public:
  drmTransmitter();
  ~drmTransmitter();
  void init(QByteArray *ba, QString name, QString format, drmTxParams params);
  void start(bool startTx);
  int getBlocks() {return blocks;}
  double transmissionTime;
private:
//  CDRMTransmitter *DRMTransmitter;
  CParameter* TransmParam ;
  int dataLength;
  double duration;
  int blocks;
};

drmTxParams modeToParams(uint mode);
uint paramsToMode(drmTxParams prm);

#endif // DRMTRANSMITTER_H
