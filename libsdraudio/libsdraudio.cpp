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
#include <fstream>

SDRAudio::SDRAudio( float sampleRate )
    :
    m_SampleRate   ( sampleRate )
{
    m_SDRLog.open( "sdr_engine_log.txt", std::ios::out );
    m_SDRLog << "SDR-Samplerate: " << m_SampleRate << "\n";

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
                                  8192 );

    m_FirFilter1 = new FirFilter( m_SampleRate,
                                  m_FilterBandwidth,
                                  0,
                                  8192 );

//    m_FirFilter1->setDeHumm();

    m_AutomaticGainControl = new AutomaticGainControl( m_SampleRate );

    m_Time = 2.0*M_PI/m_SampleRate;
    m_Phase0 = 0.0;
    m_Phase1 = 0.0;
    m_FilterToneShift = 0.f;
    m_TuneIncrement = 1;
}

void SDRAudio::updateSignalLevel( ComplexSample& input )
{
    static double lp;
    double a = exp( -2.0*M_PI*1.0/m_SampleRate );
    double b = 1.0 - a;

    lp = a * lp + b * (double)fabs(input);

    if( m_SignalLevel < (float)lp )
    {
        m_SignalLevel = (float)lp;
    }
}

void SDRAudio::setMode( enum SDR_MODE mode )
{
    m_SDRMode = mode;
}

SDRAudio::~SDRAudio()
{
    m_SDRLog.close();
    delete m_FirFilter0;
    delete m_FirFilter1;
}

bool SDRAudio::update( ComplexSample& input, ComplexSample& output )
{
    if(m_TuneFrequency > m_TargetTune ) m_TuneFrequency -= m_TuneIncrement;
    if(m_TuneFrequency < m_TargetTune ) m_TuneFrequency += m_TuneIncrement;

    m_Phase0 += (-m_TuneFrequency  )*m_Time;
    m_Phase1 += (+m_FilterToneShift)*m_Time;

    switch( m_SDRMode )
    {
        case SDR_MODE_LSB:
        case SDR_MODE_USB:
        case SDR_MODE_DSB:
        case SDR_MODE_CW:
        {
            //static std::fstream debugfile("ssb-out.raw", std::ios::binary|std::ios::out );

            output = input * ComplexSample( (float)cos(m_Phase0),
                                            (float)sin(m_Phase0) );

            output = m_FirFilter0->update( output );
            updateSignalLevel( output );
            output  = m_AutomaticGainControl->update( output );
            
            output *= ComplexSample( (float)cos(m_Phase1),
                                     (float)sin(m_Phase1) );
                                    
            output = m_FirFilter1->update( output );

            float sample = output.getI();
            output = ComplexSample( sample, sample );
            updateSquelch( output );

            //debugfile.write((char*)&output, sizeof(output) );
            return( true );
//            break;
        }
        case SDR_MODE_AM:
        {
            //static std::fstream debugfile_am("am-out.raw", std::ios::binary|std::ios::out );
            output = input * ComplexSample( (float)cos(m_Phase0),
                                            (float)sin(m_Phase0) );

            output = m_FirFilter0->update( output );
            updateSignalLevel(output);

            output = m_AutomaticGainControl->update( output );
            output *= 1.4142f*2;

            float temp = fabs(output);
            output = m_FirFilter1
                     ->update( ComplexSample( temp, temp ) );

            float sample = output.getI();
            output = ComplexSample( sample, sample );

            updateSquelch( output );
            //debugfile_am.write((char*)&output, sizeof(output) );
            return( true );
            break;
        }
        case SDR_MODE_FM:
        {
            //static std::fstream debugfile_fm("fm-out.raw", std::ios::binary|std::ios::out );

            output = input * ComplexSample( (float)cos(m_Phase0),
                                            (float)sin(m_Phase0) );

            static ComplexSample last=0.f;

            output = m_FirFilter0->update( output );

            updateSignalLevel(output);
            output = m_AutomaticGainControl->update( output );

            float value = atan2( output.getI(), output.getQ() )/M_PI-
                          atan2( last.  getI(), last  .getQ() )/M_PI;

            value = (value <= -1.0f)? value+2.0f : value;
            value = (value >= +1.0f)? value-2.0f : value;

            value *= (float)((m_SampleRate/8.0) / (m_FilterBandwidth));

            value = (value > +1.f)? +1.f:value;
            value = (value < -1.f)? -1.f:value;

            last = output;

            output = ComplexSample( value, value );
            updateSquelch( output );

            //output = ComplexSample( (float)rand()/(float)RAND_MAX,
            //                        (float)rand()/(float)RAND_MAX )*4;

            output = m_FirFilter1->update( output );

            float sample = output.getI();
            output = ComplexSample( sample, sample );

            //debugfile_fm.write((char*)&output, sizeof(output) );

            return( true );
            break;
        }
    }
    return( false );
}

void SDRAudio::setFilter( float bandwidth )
{
    float const nyquist = (float)m_SampleRate/2.f;
    float const fmax = nyquist - 200.f;
    float const fmin = 50.f;

    if( m_SDRMode == SDR_MODE_CW )
    {
        m_FilterToneShift = 1000.f;
        m_FilterFrequency = 0.f;
        bandwidth /= 2.f;
        float loFreq = (float)m_FilterToneShift-bandwidth;
        float hiFreq = (float)m_FilterToneShift+bandwidth;

        if( loFreq < fmin ) loFreq = fmin;
        if( loFreq > fmax ) loFreq = fmax;
        if( hiFreq <    0.f ) hiFreq =    0.f;
        if( hiFreq > 6000.f ) hiFreq = 6000.f;

        #if 1
        m_SDRLog << "Mode: CW ------------------------------\n";
        m_SDRLog << "IF-Shift  =    " << m_FilterToneShift << " Hz\n";
        m_SDRLog << "FLT-Shift =    " << m_FilterFrequency << " Hz\n";
        m_SDRLog << "bandwidth = +/-" << bandwidth << " Hz\n";
        m_SDRLog << "loFreq    =    " << loFreq << " Hz\n";
        m_SDRLog << "hiFreq    =    " << hiFreq << " Hz\n";
        m_SDRLog << "---------------------------------------\n\n";
        #endif

        m_FirFilter1->setDeemphLevel( -0 );
        m_FirFilter1->setDeemphFmin ( hiFreq );
        m_FirFilter1->setDeemphFmax ( hiFreq );

        m_FirFilter0->setBandwidth( -bandwidth, +bandwidth );
        m_FirFilter1->setBandwidth( loFreq, hiFreq );

        #if 1
        m_SDRLog << "Mode: CW ------------------------------\n";
        m_SDRLog << "IF-Shift  =    " << m_FilterToneShift << " Hz\n";
        m_SDRLog << "FLT-Shift =    " << m_FilterFrequency << " Hz\n";
        m_SDRLog << "bandwidth = +/-" << bandwidth << " Hz\n";
        m_SDRLog << "loFreq    =    " << loFreq << " Hz\n";
        m_SDRLog << "hiFreq    =    " << hiFreq << " Hz\n";
        m_SDRLog << "---------------------------------------\n\n";
        #endif

    }
    else
    if( m_SDRMode == SDR_MODE_LSB )
    {
        float f0 = bandwidth/2.f;
        float f1 = 50.f;
        float f2 = f1+bandwidth;

        if( f0 > fmax ) f0 = fmax;
        if( f2 > fmax ) f2 = fmax;

        m_FirFilter1->setDeemphLevel( -0 );
        m_FirFilter1->setDeemphFmin ( f0 );
        m_FirFilter1->setDeemphFmax ( f0 );

        m_FirFilter0->setBandwidth( -f2, -f1 );
        m_FirFilter1->setBandwidth( -f2, -f1 );
        m_FilterFrequency = -(f0+f1);
        m_FilterToneShift = 0.f;

        #if 1
        m_SDRLog << "Mode: LSB ------------------------------\n";
        m_SDRLog << "IF-Shift  =    " << m_FilterToneShift << " Hz\n";
        m_SDRLog << "FLT-Shift =    " << m_FilterFrequency << " Hz\n";
        m_SDRLog << "bandwidth = +/-" << bandwidth << " Hz\n";
        m_SDRLog << "loFreq    =    " << f1 << " Hz\n";
        m_SDRLog << "hiFreq    =    " << f2 << " Hz\n";
        m_SDRLog << "---------------------------------------\n\n";
        #endif
    }
    else
    if( m_SDRMode == SDR_MODE_USB )
    {
        float f0 = bandwidth/2.f;
        float f1 = 50.f;
        float f2 = f1+bandwidth;

        if( f0 > fmax ) f0 = fmax;
        if( f2 > fmax ) f2 = fmax;

        m_FirFilter1->setDeemphLevel( -0 );
        m_FirFilter1->setDeemphFmin ( f0 );
        m_FirFilter1->setDeemphFmax ( f0 );

        m_FirFilter0->setBandwidth( +f2, +f1 );
        m_FirFilter1->setBandwidth( +f2, +f1 );
        m_FilterFrequency = (f0+f1);
        m_FilterToneShift = 0.f;

        #if 1
        m_SDRLog << "Mode: USB ------------------------------\n";
        m_SDRLog << "IF-Shift  =    " << m_FilterToneShift << " Hz\n";
        m_SDRLog << "FLT-Shift =    " << m_FilterFrequency << " Hz\n";
        m_SDRLog << "bandwidth = +/-" << bandwidth << " Hz\n";
        m_SDRLog << "loFreq    =    " << f1 << " Hz\n";
        m_SDRLog << "hiFreq    =    " << f2 << " Hz\n";
        m_SDRLog << "---------------------------------------\n\n";
        #endif

    }
    else
    if( m_SDRMode == SDR_MODE_AM  ||
        m_SDRMode == SDR_MODE_DSB )
    {
        float f0 = bandwidth;
        float f1 = 75.f;

        if( f0 > fmax ) f0 = fmax;

        m_FirFilter1->setDeemphLevel( -0 );
        m_FirFilter1->setDeemphFmin ( f0 );
        m_FirFilter1->setDeemphFmax ( f0 );

        m_FirFilter0->setBandwidth( -f0, +f0 );
        m_FirFilter1->setBandwidth( +f1, +f0 );
        m_FilterFrequency = 0.f;
        m_FilterToneShift = 0.f;
        m_FilterBandwidth = f0;

    }
    else
    if( m_SDRMode == SDR_MODE_FM )
    {
        float f0 = bandwidth;
        float f1 = 50.f;

        if( f0 > fmax ) f0 = fmax;

        m_FirFilter1->setDeemphLevel( -6     );
        m_FirFilter1->setDeemphFmin ( f0/4.f );
        m_FirFilter1->setDeemphFmax ( f0     );

        m_FirFilter0->setBandwidth( -(f0+0), +(f0+0) );
        m_FirFilter1->setBandwidth( f0, f1 );
        m_FilterFrequency = 0.f;
        m_FilterToneShift = 0.f;
        m_FilterBandwidth = f0;

        #if 1
        m_SDRLog << "Mode: FM ------------------------------\n";
        m_SDRLog << "IF-Shift  =    " << m_FilterToneShift << " Hz\n";
        m_SDRLog << "FLT-Shift =    " << m_FilterFrequency << " Hz\n";
        m_SDRLog << "bandwidth = +/-" << bandwidth << " Hz\n";
        m_SDRLog << "loFreq    =    " << f1 << " Hz\n";
        m_SDRLog << "hiFreq    =    " << f0 << " Hz\n";
        m_SDRLog << "DeemphFmin=    " << f0/4 << " Hz\n";
        m_SDRLog << "DeemphFmax=    " << f0   << " Hz\n";
        m_SDRLog << "DeemphAmnt=    " << -60  << " dB\n";
        m_SDRLog << "---------------------------------------\n\n";
        #endif
    }
}

void SDRAudio::setANF( double value )
{
    m_FirFilter1->setANF( value );
}

void SDRAudio::setDNR( double value )
{
    m_FirFilter1->setDNR( value );
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
        m_AutomaticGainControl->setTime  ( upTime   );
    }
}
