//==============================================================================
//
// C++ Minimalistic-SDR-Audioprocessing-Library for the Pappradio V 2.0
//
// Copyright (C) 2011  Stefan Fendt
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the  GNU Lesser General Public License  as published by the Free
// Software Foundation;  either version 2.1 of the License,  or (at your option)
// any later version.
//
// This library is distributed  in the hope that it will be useful,  but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You  should have  received  a copy  of the  GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// =============================================================================

#ifndef __FIRFILTER_HPP__
#define __FIRFILTER_HPP__

#include <cmath>
#include "complexsample.hpp"
#include "kiss_fft.h"

class FirFilter
{
    public:
    FirFilter(  double  sampleRate,                 // no default-rate
                double  f0          = 1400.0,       // 1.4 kHz cutoff-frequency
                double  f1          = 0.0,          // 1.4 kHz cutoff-frequency
                int     length      = 127 );        // 127 taps

   ~FirFilter();

    void setBandwidth( double f0, double f1 = 0 );

    void setANF   (double value){m_ANFLevel = value;}
    void setDNR   (double value){m_DNRLevel = value;}

    ComplexSample update( ComplexSample input );

    private:
    FirFilter();

    double           m_ANFLevel;
    double           m_DNRLevel;

    int              m_FilterLength;
    double           m_SampleRate;

    int m_Pos0, m_Pos1;

    kiss_fft_cpx*    m_CoefficientsFFT;

    kiss_fft_cpx*    m_In_0;
    kiss_fft_cpx*    m_In_1;
    kiss_fft_cpx*    m_Out0;
    kiss_fft_cpx*    m_Out1;
    kiss_fft_cpx*    m_Out2;
    kiss_fft_cpx*    m_Out3;
    kiss_fft_cpx*    m_Mean;

    kiss_fft_cfg     m_KissCFGforeward;
    kiss_fft_cfg     m_KissCFGbackward;
};

#endif
