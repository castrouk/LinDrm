#ifndef FUNCTIONS_H
#define FUNCTIONS_H

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

#include <vector>
#include <numeric>

#include "drmtx/common/GlobalDefinitions.h"

/* Returns an iterator pointing to the maximum value in the vector. Does not
 * work for complex values, as they cannot be compared with operator>.
 */
template<typename T>
typename std::vector<T>::iterator vector_max(std::vector<T> & input)
{
	typename std::vector<T>::iterator result = input.begin();
	T max_val = *result;
	for(typename std::vector<T>::iterator it = result;
			it != input.end(); it++)
	{
		if(*it > max_val)
		{
			max_val = *it;
			result = it;
		}
	}
	return result;
}

/* Returns an iterator pointing to the minimum value in the vector. Does not
 * work for complex values, as they cannot be compared with operator<.
 */
template<typename T>
typename std::vector<T>::iterator vector_min(std::vector<T> & input)
{
	typename std::vector<T>::iterator result = input.begin();
	T min_val = *result;
	for(typename std::vector<T>::iterator it = result;
			it != input.end(); it++)
	{
		if(*it < min_val)
		{
			min_val = *it;
			result = it;
		}
	}
	return result;
}

/* Returns the sum of the values in the vector. Works for integer, real and
 * complex values.
 */ 
template<typename T> T vector_sum(const std::vector<T> & input)
{
	return std::accumulate(input.begin(), input.end(), 0);
}

/* Returns the sum of the squares of the values in the vector. Works for
 * integer, real and complex values.
 */ 
template<typename T> T vector_sum_sq(const std::vector<T> & input)
{
	T result = 0;
	for(typename std::vector<T>::iterator it = input.begin();
			it != input.end(); it++)
	{
		result += (*it) * (*it);
	}
	return result;
}

/* Something funny. Cannot use this function under the names sinc, sincx or
 * _sinc. Linker says it is a redefinition.
_REAL sinc(_REAL x)
{
	return x == 0.0 ? 1.0 : sin(M_PI * x) / (M_PI * x);
}
*/

void levinson(vector<_REAL> & vecrX,
		const vector<_REAL> vecrRx, const vector<_REAL> vecrB);

void levinson(vector<_COMPLEX> & veccX,
		const vector<_COMPLEX> veccRx, const vector<_COMPLEX> veccB);

#endif // FUNCTIONS_H
