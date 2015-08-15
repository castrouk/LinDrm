#ifndef FILTERS_H
#define FILTERS_H

/* FIRFilter<T> implements a Finite Impulse Response (FIR) filter. The filter
 * characteristics are described by a vector<T> of coefficients, which define
 * the filter's impulse response. The process(buf) method updates buf in place,
 * by convolving the buffer input with the filter impulse response.
 *
 * The class is templated, particularly to allow for two usages. The first,
 * used in DRMSignalIO, uses real in/out data and real filter coefficients. The
 * second, used in TimeSync, uses complex in/out data and complex filter
 * coefficients.
 *
 * The new class is intended as a partial replacement for Filter and FirFiltDec
 * from MatlibSigProToolbox.*. The main changes are to use standard library
 * vectors instead of matlib vectors, and to make the filter coefficients and
 * state buffer class members, instead of parameters to a function call. This
 * greatly simplifies the usage of the class, and gets rid of much duplication
 * of data and copying between incompatible vector types.
 *
 * The IIR feedback type of filter implemented in the matlib Filter function
 * has not been implemented, as it does not appear to be used anywhere. Only
 * the FIR type of filter is used, so this is all I provide, which results in
 * some simplification of usage.
 *
 * The decimation part of FirFiltDec has been factored out into a new Decimator
 * class. You first filter the input at the input sample rate, then use a
 * Decimator to copy every n'th sample from the filtered input data into the
 * smaller decimated output buffer.
 */

#include <vector>
#include <cmath>

#include "../GlobalDefinitions.h"

template<typename T> class FIRFilter {
public:
	FIRFilter() : buf_size_mask(0), cursor(0) {}

	void set_coeffs(const std::vector<T> & new_coeffs);

	int n_taps() const
	{ return coeffs.size(); }

	void reset_state();

	void process(std::vector<T> & in_out);

private:
	typedef typename std::vector<T> vec_type;

	std::vector<T> coeffs;
	std::vector<T> state_buf;
	int buf_size_mask;
	int cursor;
};

template<typename T>
void FIRFilter<T>::set_coeffs(const vec_type & new_coeffs)
{
	bool do_reset = new_coeffs.size() != coeffs.size();
	coeffs = new_coeffs;
	if(do_reset)
	{
		reset_state();
	}
}

template<typename T>
void FIRFilter<T>::reset_state()
{
	size_t new_size = 2;
	while(new_size < coeffs.size())
	{
		new_size <<= 1;
	}
	state_buf.resize(new_size); // Clears to 0.0 by default
	buf_size_mask = new_size - 1;
	cursor = 0;
}

template<typename T>
void FIRFilter<T>::process(std::vector<T> & in_out)
{
	for(typename std::vector<T>::iterator it = in_out.begin();
			it != in_out.end(); it++)
	{
		state_buf[cursor] = *it;
		*it = 0.0;
		int tap = cursor;
		for(typename std::vector<T>::const_iterator c_it = coeffs.begin();
				c_it != coeffs.end(); c_it++)
		{
			*it += *c_it * state_buf[tap];
			tap = (tap + 1) & buf_size_mask;
		}
		cursor = (cursor - 1) & buf_size_mask ;
	}
}

/******** One pole smoothing filters ********/

template<typename T> class OnePoleFilter {
public:
	OnePoleFilter() : m_coeff(0.0) {}

	void set_coeff(_REAL coeff)
	{ m_coeff = coeff; }

	void set_tau(_REAL tau, _REAL samp_rate)
	{ m_coeff = 1.0 - exp(-1.0 / (tau * samp_rate)); }

	void process(const T & input, T & output)
	{ output += m_coeff * (input - output); }

	void process(const vector<T> & input, vector<T> & output)
	{
		typename vector<T>::const_iterator in = input.begin();
		typename vector<T>::iterator out = output.begin();
		for( ; in != input.end(); in++, out++)
		{
			process(*in, *out);
		}
	}

private:
	_REAL m_coeff;
};


template<typename T> class DualRateOnePoleFilter {
public:
	DualRateOnePoleFilter() : m_up_coeff(0.0), m_down_coeff(0.0) {}

	void set_up_coeff(_REAL coeff)
	{ m_up_coeff = coeff; }

	void set_down_coeff(_REAL coeff)
	{ m_down_coeff = coeff; }

	void set_up_tau(_REAL tau, _REAL samp_rate)
	{ m_up_coeff = 1.0 - exp(-1.0 / (tau * samp_rate)); }

	void set_down_tau(_REAL tau, _REAL samp_rate)
	{ m_down_coeff = 1.0 - exp(-1.0 / (tau * samp_rate)); }

	void process(T & output, const T & input)
	{
		if(input > output)
		{
			output += m_up_coeff * (input - output);
		}
		else
		{
			output += m_down_coeff * (input - output);
		}
	}

	void process(const vector<T> & input, vector<T> & output)
	{
		typename vector<T>::const_iterator in = input.begin();
		typename vector<T>::iterator out = output.begin();
		for( ; in != input.end(); in++, out++)
		{
			process(*in, *out);
		}
	}

private:
	_REAL m_up_coeff;
	_REAL m_down_coeff;
};

class Decimator {
public:
	Decimator(int decimation) : m_decimation(decimation),
		input_offset(0) {}

	template<typename T>
		void process(const vector<T> & in, vector<T> & out);

private:
	unsigned int m_decimation;
	unsigned int input_offset;
};

template<typename T>
void Decimator::process(const vector<T> & in, vector<T> & out)
{
	if(input_offset >= in.size())
	{
		out.clear();
		input_offset -= in.size();
	}

	else
	{
		unsigned int out_size =
			(in.size() - input_offset) / m_decimation;
		if(out_size * m_decimation < in.size())
		{
			out_size++;
		}
		out.resize(out_size);

		typename vector<T>::const_iterator in_it =
			in.begin() + input_offset;
		typename vector<T>::iterator out_it = out.begin();
		for( ; out_it != out.end(); out_it++, in_it += m_decimation)
		{
			*out_it = *in_it;
		}

		input_offset = in_it - in.end();
//		std::cout << "input_offset " << input_offset << std::endl;
	}
}


#endif // FILTERS_H
