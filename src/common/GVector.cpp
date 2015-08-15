#include <iostream>
#include "GVector.h"

GBitVector::GBitVector() : iBitArrayCounter(0)
{
}

GBitVector::GBitVector(size_t iSize) : vector<bool>(iSize), iBitArrayCounter(0)
{
}

GBitVector::GBitVector(size_t iSize, bool filler) :
	vector<bool>(iSize, filler), iBitArrayCounter(0)
{
}

void GBitVector::Enqueue(_UINT32BIT iInformation, int iNumOfBits)
{
	if(iNumOfBits > 0)
	{
		// MSB first
		unsigned int mask = 1 << (iNumOfBits - 1);
		while(mask)
		{
	//		cerr << "GBitVector::Enqueue " << iBitArrayCounter
	//			<< ' ' << mask << endl;
			at(iBitArrayCounter++) = iInformation & mask;
			mask >>= 1;
		}
	}
}

_UINT32BIT GBitVector::Separate(int iNumOfBits)
{
	_UINT32BIT iInformation = 0;

    if((iBitArrayCounter + iNumOfBits) <= (int)size())
	{
		for(int i = 0; i < iNumOfBits; i++)
		{
			iInformation <<= 1;
			iInformation |= at(iBitArrayCounter++) & 1;
		}
	}

	return iInformation;
}
