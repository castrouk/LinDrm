/***************************************************************************
 *   Copyright (C) 2004 by Johan Maes - ON4QZ                              *
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
#ifndef FILTERPARAM_H
#define FILTERPARAM_H
#include "qsstvdefs.h"
#include <QString>

#define FILTERPARAMTYPE DSPFLOAT
//#define FILTERPARAMTYPE FILTERPARAMTYPE
 
#define RXNUMTAPS 251
#define TXWFNUMTAPS 121
#define NUMTAPSPOST 0
#define NUMRXFILTERS 1
//#define DWSBANDPASS

#ifdef DWSBANDPASS
#define DSAMPLEFILTERLEN 360
#else
#define DSAMPLEFILTERLEN 180
#endif
 
enum efilterType {FNARROW,FWIDE};
enum epostFilterType {NONE};
//extern const DSPFLOAT sharp1200BP[RXNUMTAPS];
extern const FILTERPARAMTYPE wide1200BP[RXNUMTAPS];
extern const FILTERPARAMTYPE wide600Hz[RXNUMTAPS];
//extern const DSPFLOAT wideVolume[RXNUMTAPS];
//extern const DSPFLOAT veryWideVolume[RXNUMTAPS];
//extern const DSPFLOAT narrowRX[RXNUMTAPS];
//extern const DSPFLOAT wideRX[RXNUMTAPS];
//extern const DSPFLOAT wideRXBLFIR[RXNUMTAPS];
//extern const DSPFLOAT narrowRXBLFIR[RXNUMTAPS];
extern const FILTERPARAMTYPE downSampleFilterParam[DSAMPLEFILTERLEN];
extern const  FILTERPARAMTYPE wfFilter[TXWFNUMTAPS];
//extern const  DSPFLOAT  wideVolumeFilter[RXNUMTAPS];
DSPFLOAT calculateGain(const DSPFLOAT *fp,unsigned int len);

struct sfilters
{
  const QString filterName;
  const FILTERPARAMTYPE *filterPtr;
  DSPFLOAT centerFrequency;

};

extern sfilters filterStruct[NUMRXFILTERS];


 
 #endif
