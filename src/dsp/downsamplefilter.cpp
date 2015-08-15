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
#include "downsamplefilter.h"
#include "configparams.h"
#include "utils/supportfunctions.h"
#include <QDebug>

#define VOLINTEGRATOR 0.0001

downsampleFilter::downsampleFilter()
{
	filteredDataBuffer=0;
	filterParams=0;
	samplesI=0;
	samplesQ=0;
	filterLength=0;
}

downsampleFilter::~downsampleFilter()
{
  if(filteredDataBuffer) delete [] filteredDataBuffer;
  if(filterParams) delete [] filterParams;
}

downsampleFilter::downsampleFilter(unsigned int len, const FILTERPARAMTYPE *fparam, unsigned int filterLen, bool scaled)
{
  filteredDataBuffer=0;
	filterParams=0;
	samplesI=0;
	samplesQ=0;
	filterLength=0;
  setFilterParams(fparam,filterLen,scaled);
  allocate(len);
  init();
}

void downsampleFilter::allocate(unsigned int len)
{
	length=len;
	if(filteredDataBuffer) delete [] filteredDataBuffer;
  filteredDataBuffer=new DSPFLOAT [len];
}


void downsampleFilter::init()
{
  unsigned int i;
  for(i=0;i<filterLength;i++)
    {
      samplesI[i]=0;
      samplesQ[i]=0;
    }
  for(i=0;i<length;i++)
    {
      filteredDataBuffer[i]=0;
    }
  avgVolume=0;
}

void downsampleFilter::setFilterParams(const FILTERPARAMTYPE *fparam,unsigned filterLen,bool scaled)
{
	unsigned int i;
  if(filterLen!=filterLength)
    {
      filterLength=filterLen;
      if(filterParams) delete [] filterParams;
      filterParams=new FILTERPARAMTYPE [filterLength];
      if(samplesI) delete [] samplesI;
      if(samplesQ) delete [] samplesQ;
      samplesI=new DSPFLOAT[filterLength];
      samplesQ=new DSPFLOAT[filterLength];
    }
  zeroes=filterLength-1;
  ssize=(zeroes)*sizeof(DSPFLOAT);


  DSPFLOAT gain=0;
	for(i=0;i<filterLength;i++)
		{
      gain+=fparam[i];
		}
  for(i=0;i<filterLength;i++)
    {
      if(scaled) filterParams[i]=fparam[i]/gain;
      else filterParams[i]=fparam[i];
    }
//  addToLog(QString("filtergain:=%1").arg(gain),LOGPERFORM);
}



void downsampleFilter::downSample4(short int *data)
{
  unsigned int i,k;
		FILTERPARAMTYPE res; // = float
  const FILTERPARAMTYPE *cf1;
		DSPFLOAT *fp1; // = float
  FILTERPARAMTYPE res0,res1,res2,res3;
//  int zeroes=filterLength-1;
//				qDebug("FILTERPARAMTYPE: %f, %f, %f, %f, ",res0,res1,res2,res3);
				for (k=0;k<length;k+=4)
    {
      res0=res1=res2=res3=0;
						cf1 = filterParams; // = float *filterParams
						fp1 = samplesI; // = float *samplesI
      memmove(samplesI+4, samplesI, (filterLength-4)*sizeof(DSPFLOAT));
						samplesI[3]= data[k];
      samplesI[2]= data[k+1];
      samplesI[1]= data[k+2];
      samplesI[0]= data[k+3];
      for(i=0;i<filterLength;i+=4,fp1+=4,cf1+=4)
        {
          res0+=(*fp1)*(*cf1);
          res1+=(*(fp1+1))*(*(cf1+1));
          res2+=(*(fp1+2))*(*(cf1+2));
          res3+=(*(fp1+3))*(*(cf1+3));
        }
      res=res0+res1+res2+res3;
//						qDebug() << "res: " << res;
						filteredDataBuffer[k/4]=(DSPFLOAT)res;
//        filteredDataBuffer[k/4]=samplesI[filterLength/2];
						avgVolume=avgVolume*(1-VOLINTEGRATOR)+(10*log(res*res)-145)*VOLINTEGRATOR;

    }
//		qDebug() << "avgVolume: " << avgVolume;
}





