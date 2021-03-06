#ifndef FFT_H
#define FFT_H

/******************************************************************************\
 * Copyright (c) 2009 Glyn Adgie
 *
 * Author(s):
 *	Glyn Adgie
 *
 * Description:
 *	C++ interfaces for fftw3 functions. Windowing functions.
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

#include <fftw3.h>
#include <vector>

#include "drmtx/common/GlobalDefinitions.h"

class ComplexFFT {
public:
	ComplexFFT() {}

	~ComplexFFT()
	{ fftw_destroy_plan(plan); }

	void init(int size, int direction);

	void execute()
	{ fftw_execute(plan); }

	vector<_COMPLEX> & input()
	{ return m_in; }

	vector<_COMPLEX> & output()
	{ return m_out; }

private:
	vector<_COMPLEX> m_in;
	vector<_COMPLEX> m_out;
	fftw_plan plan;
};

class FFTr2c {
public:
	FFTr2c() {}

	~FFTr2c()
	{ fftw_destroy_plan(plan); }

	void init(int size);

	void execute()
	{ fftw_execute(plan); }

	vector<_REAL> & input()
	{ return m_in; }

	vector<_COMPLEX> & output()
	{ return m_out; }

private:
	vector<_REAL> m_in;
	vector<_COMPLEX> m_out;
	fftw_plan plan;
};

class FFTc2r {
public:
	FFTc2r() {}

	~FFTc2r()
	{ fftw_destroy_plan(plan); }

	void init(int size);

	void execute()
	{ fftw_execute(plan); }

	vector<_COMPLEX> & input()
	{ return m_in; }

	vector<_REAL> & output()
	{ return m_out; }

private:
	vector<_COMPLEX> m_in;
	vector<_REAL> m_out;
	fftw_plan plan;
};

/*
class FFTWindow {
public:
	void make_hamming(int size);

	void make_inverse_hamming(int size);

	void make_blackman(int size);

	template<typename T> void process(vector<T> & in_out)
	{
		for(unsigned int i = 0; i < m_window.size(); i++)
		{
			in_out[i] *= m_window[i];
		}
	}

	template<typename T> void process(const vector<T> & input,
			vector<T> & output)
	{
		for(unsigned int i = 0; i < m_window.size(); i++)
		{
			output[i] = input[i] * m_window[i];
		}
	}

private:
	vector<_REAL> m_window;
};
*/

void make_hamming_vec(vector<_REAL> & vec);

void make_inverse_hamming_vec(vector<_REAL> & vec);

void make_blackman_vec(vector<_REAL> & vec);

#endif // FFT_H
