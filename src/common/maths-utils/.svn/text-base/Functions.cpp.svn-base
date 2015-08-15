/******************************************************************************\
 * Copyright (c) 2001 Volker Fischer
 * Copyright (c) 2009 Glyn Adgie
 *
 * Author(s):
 *	Volker Fischer, Glyn Adgie
 *
 * Description:
 *	Re-implementation of some functions from C++ Mathematic Library
 *	(Matlib). Use std::vector instead of Matlib vectors.
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include "Functions.h"


void levinson(vector<_REAL> & vecrX,
		const vector<_REAL> vecrRx, const vector<_REAL> vecrB)
{
	/* 
	The Levinson recursion [S. Haykin]

	This algorithm solves the following equations:
	Rp ap = ep u1,
	Rp Xp = b, where Rp is a Toepliz-matrix of vector prRx and b = prB 
	is an arbitrary correlation-vector. The Result is ap = prA.

	Parts of the following code are taken from Ptolemy
	(http://ptolemy.eecs.berkeley.edu/)
	*/

	int iLength = vecrRx.size();
	vecrX.resize(iLength);
	vector<_REAL> vecraP(iLength);
	vector<_REAL> vecrA(iLength);
	_REAL rGamma;
	_REAL rGammaCap;
	_REAL rDelta;
	_REAL rE;
	_REAL rQ;

	/* Initialize the recursion */

	// (a) First coefficient is always unity
	vecrA[0] = 1.0;
	vecraP[0] = 1.0;

	// (b) 
	vecrX[0] = vecrB[0] / vecrRx[0];

	// (c) Initial prediction error is simply the zero-lag of
	// of the autocorrelation, or the signal power estimate.
	rE = vecrRx[0];

	/* Main loop */
	for (int j = 0; j < iLength - 1; j++)
	{
		int iNextInd = j + 1;

		// (a) Compute the new gamma
		rGamma = vecrRx[iNextInd];
		for (int i = 1; i < iNextInd; i++) 
		{
			rGamma += vecrA[i] * vecrRx[iNextInd - i];
		}

		// (b), (d) Compute and output the reflection coefficient
		// (which is also equal to the last AR parameter)
		vecrA[j + 1] = rGammaCap = - rGamma / rE;

		// (c)
		for (int i = 1; i < iNextInd; i++)
		{
			vecraP[i] = vecrA[i] + rGammaCap * vecrA[iNextInd - i];
		}

		// Swap a and aP for next order recurrence
		for (int i = 1; i < iNextInd; i++)
		{
			vecrA[i] = vecraP[i];
		}

		// (e) Update the prediction error power
		rE *= 1.0 - rGammaCap * rGammaCap;

		rDelta = 0.0;
		for (int i = 0; i < iNextInd; i++)
		{
			rDelta += vecrX[i] * vecrRx[iNextInd - i];
		}

		// (g), (i) 
		vecrX[iNextInd] = rQ = (vecrB[iNextInd] - rDelta) / rE;

		// (h)
		for (int i = 0; i < iNextInd; i++) 
		{
			vecrX[i] += rQ * vecrA[iNextInd - i];
		}
	}
}

void levinson(vector<_COMPLEX> & veccX,
		const vector<_COMPLEX> veccRx, const vector<_COMPLEX> veccB)
{
	/* 
	The levinson recursion [S. Haykin]
	COMPLEX version.

	This algorithm solves the following equations:
	Rp ap = ep u1,
	Rp Xp = b, where Rp is a Toepliz-matrix of vector prRx and b = prB 
	is an arbitrary correlation-vector. The Result is ap = prA.

	Parts of the following code are taken from Ptolemy
	(http://ptolemy.eecs.berkeley.edu/)
*/

	int iLength = veccRx.size();
//	CComplexVector	veccX(iLength, VTY_TEMP);

	_COMPLEX cGamma;
	_COMPLEX cGammaCap;
	_COMPLEX cDelta;
	_REAL rE;
	_COMPLEX cQ;
	int i, j;
	vector<_COMPLEX> veccaP(iLength);
	vector<_COMPLEX> veccA(iLength);

	/******** Initialize the recursion ********/
	// (a) First coefficient is always unity
	veccA[0] = 1.0;
	veccaP[0] = 1.0;

	// (b) 
	veccX[0] = veccB[0] / veccRx[0];

	// (c) Initial prediction error is simply the zero-lag of
	// of the autocorrelation, or the signal power estimate.
	rE = veccRx[0].real();

	/******** Main loop ********/
	// The order recurrence
	for (j = 0; j < iLength - 1; j++)
	{
		const int iNextInd = j + 1;

		// (a) Compute the new gamma
		cGamma = veccRx[iNextInd];
		for (i = 1; i < iNextInd; i++) 
			cGamma += veccA[i] * veccRx[iNextInd - i];

		// (b), (d) Compute and output the reflection coefficient
		// (which is also equal to the last AR parameter)
		veccA[iNextInd] = cGammaCap = - cGamma / rE;

		// (c)
		for (i = 1; i < iNextInd; i++) 
			veccaP[i] = veccA[i] + cGammaCap * conj(veccA[iNextInd - i]);

		// Swap a and aP for next order recurrence
		for (i = 1; i < iNextInd; i++)
			veccA[i] = veccaP[i];

		// (e) Update the prediction error power
		rE *= 1.0 - norm(cGammaCap);

		// (f)
		cDelta = 0.0;
		for(i = 0; i < iNextInd; i++)
		{
			cDelta += veccX[i] * veccRx[iNextInd - i];
		}

		// (g), (i) 
		veccX[iNextInd] = cQ = (veccB[iNextInd] - cDelta) / rE;

		// (h)
		for(i = 0; i < iNextInd; i++)
		{	
			veccX[i] = veccX[i] + cQ * conj(veccA[iNextInd - i]);
		}
	}


}


