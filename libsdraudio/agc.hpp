//==============================================================================
//
// C++ Minimalistic-SDR-Audioprocessing-Library for the Pappradio V 2.0
//
// Copyright (C) 2011, 2012  Stefan Fendt
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

#ifndef __AGC_HPP__
#define __AGC_HPP__

#include <cmath>
#include "complexsample.hpp"

class   AutomaticGainControl
{
    public:

    // ========================================================================
    // constructor
    // ========================================================================

    AutomaticGainControl    ( double samplerate, double time   = 0.5 );

    // ========================================================================
    // constructor
    // ========================================================================

   ~AutomaticGainControl    ();

    void            setTime ( double Time );

    ComplexSample   update  ( ComplexSample& input );

    private:
    AutomaticGainControl();

    double          m_SampleRate;

    float           m_TargetLevel;
    float           m_LimitLevel;

    long double     m_Gain;
    long double     m_LimitGain;
    long double     m_GainUp;
    long double     m_GainDown;

    float           m_Level;
    float           m_LastLevel;
    float           m_ThisLevel;
    float           m_Level0;
    float           m_Level1;
    float           m_Level2;
    float           m_Level3;

    int             m_DataLength;
    int             m_DataPosition;

    ComplexSample*  m_Data;
};

#endif
