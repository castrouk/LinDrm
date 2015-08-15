Mathematics utilities
----------------------
Glyn Adgie. 30 Nov 09

These utilities are intended as replacements for the various functions and data
types in the current matlib files. There are several reasons for doing this.

1) Replacement of custom matlib vector types with std::vector. This is part of
the general policy of using std::vector throughout the code, as in most cases,
the custom vector types add little functionality over std::vector, and are
often incompatible with normal STL coding styles.

2) Update FFT functionality to use fftw3 instead of the obsolete fftw2.

3) Remove functions that merely duplicate what is already available in the C++
standard library. For example SqMag(complex_num) does the same as
std::norm(complex_num).

4)  


