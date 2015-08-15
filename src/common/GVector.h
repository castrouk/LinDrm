#ifndef GVECTOR_H
#define GVECTOR_H

#include "drmtx/common/GlobalDefinitions.h"
#include <vector>
#include <algorithm>
#include <QDebug>

typedef uint64_t _UINT64BIT;
typedef uint32_t _UINT32BIT;
typedef uint16_t _UINT16BIT;

class GBitVector : public vector<bool> {
public:
	GBitVector();

	GBitVector(size_t iSize);

	GBitVector(size_t iSize, bool filler);

	void Enqueue(_UINT32BIT iInformation, int iNumOfBits);

	_UINT32BIT Separate(int iNumOfBits);

	void ResetBitAccess()
	{ iBitArrayCounter = 0; }

private:
    int iBitArrayCounter;
};

template<typename T> class GVectorEx : public vector<T> {
public:
	int SymbolID() const
	{ return m_SymbolID; }

	void SetSymbolID(int n)
	{ m_SymbolID = n; }

	int CurTimeCorr() const
	{ return m_CurTimeCorr; }

	void SetCurTimeCorr(int n)
	{ m_CurTimeCorr = n; }

private:
	/* Symbol ID of the current block. This number only identifies the
	   position in a frame, NOT in a super-frame */
	int m_SymbolID;

	/* The channel estimation needs information about timing corrections, 
	   because it is using information from the symbol memory */
	int m_CurTimeCorr;
};

template<> class GVectorEx<_BINARY> : public GBitVector {
public:
	int SymbolID() const
	{ return m_SymbolID; }

	void SetSymbolID(int n)
	{ m_SymbolID = n; }

	int CurTimeCorr() const
	{ return m_CurTimeCorr; }

	void SetCurTimeCorr(int n)
	{ m_CurTimeCorr = n; }

private:
	/* Symbol ID of the current block. This number only identifies the
	   position in a frame, NOT in a super-frame */
	int m_SymbolID;

	/* The channel estimation needs information about timing corrections, 
	   because it is using information from the symbol memory */
	int m_CurTimeCorr;

};

template<typename T> class GShiftRegister : public vector<T> {
public:
	GShiftRegister() {}

	GShiftRegister(size_t iSize) : vector<T>(iSize) {}

	GShiftRegister(size_t iSize, bool filler) : vector<T>(iSize, filler) {}

	/* Add one value at the beginning, shift the others to the right */
	void AddBegin(const T & tNewD);

	/* Add one value at the end, shift the others to the left */
	void AddEnd(const T & tNewD);

	/* Add a vector at the end, shift others to the left */
	void AddEnd(const vector<T> & vectNewD, int iLen);
};

/* GShiftRegister implementation */

template<class T> void GShiftRegister<T>::AddBegin(const T & tNewD)
{
//	cerr << "GShiftRegister<T>::AddBegin\n";
	typename vector<T>::iterator it;

	/* Shift old values */
	for(it = this->end() - 1; it != this->begin(); it--)
	{
		*it = *(it - 1);
	}

	/* Add new value */
	*it = tNewD;
}

template<class T> void GShiftRegister<T>::AddEnd(const T & tNewD)
{
	typename vector<T>::iterator it;
	/* Shift old values */
	for(it = this->begin(); it != this->end() - 1; it++)
	{
		*it = *(it + 1);
	}

	/* Add new value */
	*it = tNewD;
}

template<class T>
void GShiftRegister<T>::AddEnd(const vector<T> & vectNewD, int iLen)
{
	typename vector<T>::iterator it;
//		qDebug() << "iLen:" << iLen;
	/* Shift old values */
	for(it = this->begin(); it != this->end() - iLen; it++)
	{
		*it = *(it + iLen);
	}

	/* Add new block of data */
	copy(vectNewD.begin(), vectNewD.begin() + iLen, it);
}

template<typename T> class GMatrix {
public:
	typedef typename vector<vector<T> >::iterator iterator;
	typedef typename vector<vector<T> >::const_iterator const_iterator;

	GMatrix() {}

	GMatrix(int iNewR, int iNewC)
	{ Init(iNewR, iNewC); }

	GMatrix(int iNewR, int iNewC, const T & filler)
	{ Init(iNewR, iNewC, filler); }

	int n_rows()
	{ return vvData.size(); }

	int n_cols()
	{
		if(vvData.empty())
		{
			return 0;
		}
		return vvData[0].size();
	}

	iterator begin()
	{ return vvData.begin(); }

	const_iterator begin() const
	{ return vvData.begin(); }

	iterator end()
	{ return vvData.end(); }

	const_iterator end() const
	{ return vvData.end(); }


	vector<T> & back()
	{ return vvData.back(); }

	const vector<T> & back() const
	{ return vvData.back(); }


	void Init(int iNewRow, int iNewColumn)
	{
		vvData.resize(iNewRow);
		typename vector<vector<T> >::iterator row_it;
		for(row_it = vvData.begin(); row_it != vvData.end(); row_it++)
		{
			row_it->resize(iNewColumn);
		}
	}

	void Init(int iNewRow, int iNewColumn, const T & filler)
	{
		vvData.resize(iNewRow);
		typename vector<vector<T> >::iterator row_it;
		for(row_it = vvData.begin(); row_it != vvData.end(); row_it++)
		{
			row_it->resize(iNewColumn, filler);
		}
	}

	void Reset(const T & filler)
	{
		typename vector<vector<T> >::iterator row_it;
		for(row_it = vvData.begin(); row_it != vvData.end(); row_it++)
		{
			fill(row_it->begin(), row_it.end(), filler);
		}
	}

	vector<T> & operator[](int iPos)
	{ return vvData[iPos]; }

	const vector<T> & operator[](int iPos) const
	{ return vvData[iPos]; }


private:
	vector<vector<T> > vvData;
//	int iRow;
};

#endif
