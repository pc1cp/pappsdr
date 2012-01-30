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

#include "libsdraudio.hpp"

#include "firfilter.hpp"
#include "agc.hpp"

#include <iostream>

SDRAudio::SDRAudio( float sampleRate )
    :
    m_SampleRate   ( sampleRate )
{
    std::cerr << "SDR-Input-Samplerate .... : " << m_SampleRate << "\n";

    m_TuneFrequency         =    0;
    m_FilterFrequency       = 1500;
    m_FilterBandwidth       = 1400;
    m_SDRMode               = SDR_MODE_LSB;

    m_SignalLevel = 0.f;
    m_SquelchLevel = 0.f;
    m_SquelchCount = 12000;

    m_FirFilter0 = new FirFilter( m_SampleRate,
                                  m_FilterBandwidth,
                                  0,
                                  4096 );

    m_FirFilter1 = new FirFilter( m_SampleRate,
                                  m_FilterBandwidth,
                                  0,
                                  4096 );

    m_AutomaticGainControl = new AutomaticGainControl( m_SampleRate );
}

void SDRAudio::updateSignalLevel( ComplexSample& input )
{
    if( m_SignalLevel < fabs(input) )
    {
        m_SignalLevel = fabs(input);
    }
}

void SDRAudio::setMode( enum SDR_MODE mode )
{
    m_SDRMode = mode;
}

SDRAudio::~SDRAudio()
{
    delete m_FirFilter0;
    delete m_FirFilter1;
}

#include <fstream>

bool SDRAudio::update( ComplexSample& input, ComplexSample& output )
{
    m_Time = 1.0 / m_SampleRate;//fmod( m_Time + 1.0 / m_InputSampleRate, 1.0 );

    static double phase0;
    static double phase1;
    phase0 += m_Time*(m_TuneFrequency  -m_FilterFrequency)*-2.f*M_PI;
    phase1 += m_Time*(m_FilterFrequency+m_FilterToneShift)*-2.f*M_PI;

    switch( m_SDRMode )
    {
        case SDR_MODE_LSB:
        case SDR_MODE_USB:
        case SDR_MODE_DSB:
        case SDR_MODE_CW:
        {
            //static std::fstream debugfile("ssb-out.raw", std::ios::binary|std::ios::out );

            output = input * ComplexSample( (float)cos(phase0),
                                            (float)sin(phase0) );
            output = m_FirFilter0->update( output );
            updateSignalLevel( output );
            output  = m_AutomaticGainControl->update( output );
            output *= ComplexSample( (float)cos(phase1),
                                     (float)sin(phase1) );
            //float sample = output.getI();
            //output = ComplexSample( sample, sample );
            updateSquelch( output );
            //output = m_FirFilter1->update( output );

            //debugfile.write((char*)&sample, sizeof(sample) );
            return( true );
//            break;
        }
        case SDR_MODE_AM:
        {
            output = input * ComplexSample( (float)cos(phase0),
                                            (float)sin(phase0) );

            output = m_FirFilter0->update( output );
            updateSignalLevel(output);

            float temp = fabs(output);
            output = m_FirFilter1
                     ->update( ComplexSample( temp, temp ) );

            output = m_AutomaticGainControl->update( output );
            output *= 1.4142;

            updateSquelch( output );
            return( true );
            break;
        }
        case SDR_MODE_FM:
        {
            output = input * ComplexSample( (float)cos(phase0),
                                            (float)sin(phase0) );
            static ComplexSample last0;
            static ComplexSample last1;

            output = m_FirFilter0->update( output );
            updateSignalLevel(output);

            output /= fabs(output);

            float value0 = atan2( output.getI(), output.getQ() )/M_PI-
                           atan2( last0. getI(), last0 .getQ() )/M_PI;

            float value1 = atan2( output.getI(), output.getQ() )/M_PI-
                           atan2( last1. getI(), last1 .getQ() )/M_PI;

            value0 = (value0<=-1.0)? value0+2.0:value0;
            value0 = (value0>=+1.0)? value0-2.0:value0;

            value1 = (value1<=-1.0)? value1+2.0:value1;
            value1 = (value1>=+1.0)? value1-2.0:value1;

            float value = (value0)/1.0;

            value *= m_SampleRate / m_FilterBandwidth;
            value = (value > +1.f)? +1.f:value;
            value = (value < -1.f)? -1.f:value;

            last1 = last0;
            last0 = output;

            output = ComplexSample( value, value );
            updateSquelch( output );

            output = m_FirFilter1->update( output );

            return( true );
            break;
        }
    }
    return( false );
}

void SDRAudio::setFilter( float bandwidth )
{
    float const nyquist = m_SampleRate/2.f;
    float const fmax = nyquist - 200.f;
    float const fmin = 50.f;

    if( m_SDRMode == SDR_MODE_CW )
    {
        m_FilterToneShift = 1000.f;
        m_FilterFrequency = 0.f;
        bandwidth /= 2.f;
        float loFreq = m_FilterToneShift-bandwidth;
        float hiFreq = m_FilterToneShift+bandwidth;

        if( loFreq < fmin ) loFreq = fmin;
        if( loFreq > fmax ) loFreq = fmax;
        if( hiFreq <    0.f ) hiFreq =    0.f;
        if( hiFreq > 6000.f ) hiFreq = 6000.f;

        std::cerr << "bandwidth = +/-" << bandwidth << " Hz\n";
        std::cerr << "loFreq    =    " << loFreq << " Hz\n";
        std::cerr << "hiFreq    =    " << hiFreq << " Hz\n";

        m_FirFilter0->setBandwidth( bandwidth );
        m_FirFilter1->setBandwidth( loFreq, hiFreq );
    }
    else
    if( m_SDRMode == SDR_MODE_LSB )
    {
        float f0 = bandwidth/2.f;
        float f1 = 200.f;
        float f2 = f1+bandwidth;

        if( f0 > fmax ) f0 = fmax;
        if( f2 > fmax ) f2 = fmax;

        m_FirFilter0->setBandwidth( f0 );
        m_FirFilter1->setBandwidth( f2, f1 );
        m_FilterFrequency = f0+f1;
        m_FilterToneShift = 0.f;
    }
    else
    if( m_SDRMode == SDR_MODE_USB )
    {
        float f0 = bandwidth/2.f;
        float f1 = 200.f;
        float f2 = f1+bandwidth;

        if( f0 > fmax ) f0 = fmax;
        if( f2 > fmax ) f2 = fmax;

        m_FirFilter0->setBandwidth( f0 );
        m_FirFilter1->setBandwidth( f2, f1 );
        m_FilterFrequency = -(f0+f1);
        m_FilterToneShift = 0.f;
    }
    else
    if( m_SDRMode == SDR_MODE_AM  ||
        m_SDRMode == SDR_MODE_FM  ||
        m_SDRMode == SDR_MODE_DSB )
    {
        float f0 = bandwidth;
        float f1 = 50.f;

        if( f0 > fmax ) f0 = fmax;

        m_FirFilter0->setBandwidth( f0*2.f );
        m_FirFilter1->setBandwidth( f0, f1 );
        m_FilterFrequency = 0.f;
        m_FilterToneShift = 0.f;
        m_FilterBandwidth = f0;
    }
}

ComplexSample SDRAudio::demodulate( ComplexSample& inputSample )
{
    ComplexSample tmp;
    update( inputSample, tmp );
    return( tmp );
}

float SDRAudio::getSignalLevel()
{
    float returnValue = m_SignalLevel;
    m_SignalLevel = 0.f;
    return( returnValue );
}

float SDRAudio::getSquelchLevel()
{
    return( m_SquelchLevel );
}

void SDRAudio::setSquelchLevel( float level )
{
    m_SquelchLevel = level;
}

void SDRAudio::setAGCTime( double upTime, double downTime )
{
    if( m_AutomaticGainControl )
    {
        m_AutomaticGainControl->setUpTime  ( upTime   );
        m_AutomaticGainControl->setDownTime( downTime );
    }
}
