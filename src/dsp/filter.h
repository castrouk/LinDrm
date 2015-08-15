/***************************************************************************
 *   Copyright (C) 2000-2008 by Johan Maes                                 *
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
#ifndef FILTER_H
#define FILTER_H
#include "nco.h"
#include "filterparam.h"


class filter
{
public:
	filter();
  filter(unsigned int len,const FILTERPARAMTYPE *fparam,unsigned int filterLen,DSPFLOAT centerFrequency,DSPFLOAT clock,bool scaled,DSPFLOAT volumeIntegrator);
	~filter();
	void allocate(unsigned int dataLength);
  void setFilterParams(const FILTERPARAMTYPE *fparam,unsigned int filterLen,DSPFLOAT centerFrequency,DSPFLOAT clock,bool scaled,DSPFLOAT volumeIntegrator);
  void processFM(DSPFLOAT *data);
  void processBP(DSPFLOAT *data);
  void processAM(DSPFLOAT *data);
  void processIQ(DSPFLOAT *data,DSPFLOAT *output, int len);
  int *filteredDataPtr() {return filteredDataBuffer;}
  int *volumePtr() {return volumeBuffer;}
  void process(DSPFLOAT *data,DSPFLOAT *filteredData,int len);
  void delay(DSPFLOAT *data,DSPFLOAT *filteredData,int len);

private:
	void normalizeGain();
	bool demoluteFM;
  int *filteredDataBuffer;
  int *volumeBuffer;

  FILTERPARAMTYPE *filterParams;
	unsigned int filterLength;
	DSPFLOAT *samplesI;
	DSPFLOAT *samplesQ;
  FILTERPARAMTYPE	resIprev;
  FILTERPARAMTYPE	resQprev;
  int prevTemp;
	unsigned int length;
	NCO nco;
	DSPFLOAT frCenter;
	DSPFLOAT angleToFc;
  DSPFLOAT holdPrev;
  DSPFLOAT amplitude;
  int zeroes;
  int ssize;
  DSPFLOAT avgVolume;
  DSPFLOAT volIntegrator;
};

#endif
