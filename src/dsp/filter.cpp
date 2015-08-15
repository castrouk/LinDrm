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
#include "filter.h"
#include "configparams.h"





filter::filter()
{
	filteredDataBuffer=0;
	volumeBuffer=0;
	filterParams=0;
	samplesI=0;
	samplesQ=0;
	filterLength=0;
}

filter::~filter()
{
  if(filteredDataBuffer) delete [] filteredDataBuffer;
  if(volumeBuffer) delete [] volumeBuffer;
  if(filterParams) delete [] filterParams;
}

filter::filter(unsigned int len, const FILTERPARAMTYPE *fparam, unsigned int filterLen, DSPFLOAT centerFrequency, DSPFLOAT clock, bool scaled, DSPFLOAT volumeIntegrator)
{
  filteredDataBuffer=0;
  volumeBuffer=0;
	filterParams=0;
	samplesI=0;
	samplesQ=0;
	filterLength=0;
	allocate(len);
  setFilterParams(fparam,filterLen,centerFrequency,clock,scaled,volumeIntegrator);
  resIprev=0;
  resQprev=0;
}

void filter::allocate(unsigned int len)
{
	length=len;
	if(filteredDataBuffer) delete [] filteredDataBuffer;
	if(volumeBuffer) delete [] volumeBuffer;
  filteredDataBuffer=new int [2*len]; // double the length for complex output in IQ filtering -output complex format
  volumeBuffer=new int [len];
}

void filter::setFilterParams(const FILTERPARAMTYPE *fparam,unsigned filterLen,DSPFLOAT centerFrequency,DSPFLOAT clock,bool scaled,DSPFLOAT volumeIntegrator)
{
	unsigned int i;
  volIntegrator=volumeIntegrator;
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
  frCenter=centerFrequency;
  nco.init(frCenter/clock);
  angleToFc=clock/(2*M_PI);


	for(i=0;i<filterLength;i++)
		{
			samplesI[i]=0;
			samplesQ[i]=0;
		}
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
  avgVolume=0;
//  addToLog(QString("filtergain:=%1").arg(gain),LOGPERFORM);
}

void filter::processFM(DSPFLOAT *data)
{
  FILTERPARAMTYPE resI=0, resQ=0;
  int  temp;

  const FILTERPARAMTYPE *cf1;
  DSPFLOAT *fp1, *fp2;
  FILTERPARAMTYPE discRe,discIm;
	unsigned int i,k;


	for (k=0;k<length;k++)
		{
			resI=0;
			resQ=0;
			cf1 = filterParams;
			fp1 = samplesI;
			fp2 = samplesQ;
      memmove(samplesI, samplesI+1, ssize);
      memmove(samplesQ, samplesQ+1, ssize);
      nco.multiply(samplesI[zeroes],samplesQ[zeroes],data[k]);
			for(i=0;i<filterLength;i++,fp1++,fp2++,cf1++)
 				{
   				resI+=(*fp1)*(*cf1);
   				resQ+=(*fp2)*(*cf1);
 				}
			discRe=resI*resIprev+resQ*resQprev;
  		discIm=-resQ*resIprev+resQprev*resI;
  		resIprev=resI;
  		resQprev=resQ;
  		if(discRe==0) discRe=0.0001;
      temp=(int)round(frCenter-atan2(discIm,discRe)*angleToFc);
      if(temp<500) temp=prevTemp;
      if(temp>2600) temp=prevTemp;
      prevTemp=temp;
			filteredDataBuffer[k]=temp;
      avgVolume=avgVolume*(1-volIntegrator)+sqrt(resI*resI+resQ*resQ)*volIntegrator;
      volumeBuffer[k]=(int)avgVolume;
		}
}

void filter::processIQ(DSPFLOAT *data, DSPFLOAT *output,int len)
{
  FILTERPARAMTYPE resQ=0;

  const FILTERPARAMTYPE *cf1;
  DSPFLOAT *fp1;
  unsigned int i;
  int k;


  for (k=0;k<len;k++)
    {
      resQ=0;
      cf1 = filterParams;
      fp1 = samplesI;
      memmove(samplesI+1, samplesI, ssize); // newest at index 0
      samplesI[0]=data[k];
      for(i=0;i<filterLength;i++,fp1++,cf1++)
        {
          resQ+=(*fp1)*(*cf1);
        }
      output[2*k+1]=samplesI[filterLength/2]; // just delay
//      output[2*k]=resQ*0.911;
      output[2*k]=(DSPFLOAT)resQ;
    }
}


void filter::processAM(DSPFLOAT *data)
{
  FILTERPARAMTYPE resI=0, resQ=0;
  const FILTERPARAMTYPE *cf1;
	DSPFLOAT *fp1, *fp2 ,amplitude;
	unsigned int i,k;
	int zeroes=filterLength-1;

	for (k=0;k<length;k++)
		{
			resI=0;
			resQ=0;
			cf1 = filterParams;
			fp1 = samplesI;
			fp2 = samplesQ;
			memmove(samplesI, samplesI+1, (zeroes)*sizeof(DSPFLOAT));
  		memmove(samplesQ, samplesQ+1, (zeroes)*sizeof(DSPFLOAT));
      nco.multiply(samplesI[zeroes],samplesQ[zeroes],data[k]);
			for(i=0;i<filterLength;i++,fp1++,fp2++,cf1++)
 				{
   				resI+=(*fp1)*(*cf1);
   				resQ+=(*fp2)*(*cf1);
 				}
			amplitude=sqrt(resI*resI+resQ*resQ);
      filteredDataBuffer[k]=2*amplitude;
		}
}

void filter::process(DSPFLOAT *data,DSPFLOAT *filteredData,int len)
{
  FILTERPARAMTYPE resI=0;
  const FILTERPARAMTYPE *cf1;
  DSPFLOAT *fp1;
  unsigned int i;
  int k;
  int zeroes=filterLength-1;
  for (k=0;k<len;k++)
    {
      resI=0;
      cf1 = filterParams;
      fp1 = samplesI;
      memmove(samplesI+1, samplesI, (zeroes)*sizeof(DSPFLOAT));
      samplesI[0]=data[k];
      for(i=0;i<filterLength;i++,fp1++,cf1++)
        {
          resI+=(*fp1)*(*cf1);
        };
      filteredData[k]=(DSPFLOAT)resI;

    }
}


void filter::delay(DSPFLOAT *data,DSPFLOAT *filteredData,int len)
{
  int k;
  int zeroes=filterLength-1;
  for (k=0;k<len;k++)
    {
      memmove(samplesI, samplesI+1, (zeroes)*sizeof(DSPFLOAT));
      samplesI[zeroes]=data[k];
      filteredData[k]=samplesI[filterLength/2];
    }
}

