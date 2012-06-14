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

#ifndef __LIBSDRAUDIO_HPP__
#define __LIBSDRAUDIO_HPP__

#include <fstream>
#include <cstdlib>
#include "kiss_fft.h"
#include <iostream>

// forward declarations of some classes

//class ComplexSample;
#include "complexsample.hpp"
class Downsampler;
class FirFilter;
class AutomaticGainControl;

#if 0
class AutomaticGainControl
{
    static const float _sampleRate = 12000;
    static const float upTime   = 1.50;

    public:
    AutomaticGainControl()
    {
        m_Gain        = 1.0;
        m_TargetLevel = 0.875;

        m_DelaySize   = _sampleRate / 25;
        m_Delay       = new ComplexSample[m_DelaySize];

        m_UpGain = pow( 2.0, 1.0/(upTime*_sampleRate) );
        std::cerr << "upGain=" << m_UpGain << "\n";
        m_Position = 0;
    }

    ~AutomaticGainControl()
    {
        delete [] m_Delay;
    }

    ComplexSample update( ComplexSample input )
    {
        //input = 1.0;
        static std::fstream file("debug.dat",std::ios::out|std::ios::binary);

        m_Position = (m_Position+1)%m_DelaySize;
        m_Delay[ m_Position%m_DelaySize ] = input;

        float max=0;
        for( int n=0; n<m_DelaySize; ++n )
        {
            if(max<fabs(m_Delay[n]))
                max=fabs(m_Delay[n]);
        }

        if( max != 0 )
        {
            float Gain = m_TargetLevel / max;
            if( m_Gain > Gain )
                m_Gain = Gain;
            else
                m_Gain *= m_UpGain;
        }

        if( m_Gain < 1.0 )
            m_Gain = 1.0;
        //std::cerr << "\rGain:" << m_Gain;

        ComplexSample out = m_Delay[ (m_Position+1)%m_DelaySize ]*m_Gain;

        file.write( (char*)&out,          sizeof(out         ) );

        return( out );
    }

    private:

    float       m_TargetLevel;
    float       m_AGCUptime;
    float       m_AGCDowntime;
    float       m_UpGain;
    float       m_DownGain;
    float       m_MeanLevel;
    float       m_PeakLevel0;
    float       m_PeakLevel1;
    float       m_PeakLevel2;
    float       m_PeakLevel3;
    float       m_Gain;

    int             m_DelaySize;
    ComplexSample*  m_Delay;
    int             m_Position;
};
#endif

class SDRAudio
{
    public:

    enum SDR_MODE
    {
        SDR_MODE_CW,
        SDR_MODE_LSB,
        SDR_MODE_USB,
        SDR_MODE_DSB,
        SDR_MODE_AM,
        SDR_MODE_FM
    };

    SDRAudio( float sampleRate );
   ~SDRAudio();

    void setTune( float frequency )
    {
        double delta = fabs( m_TuneFrequency - frequency );
        m_TargetTune = frequency;

        m_TuneIncrement = delta/(m_SampleRate*0.25);

        if( delta < 200 )
        {
            m_TuneIncrement /= 2.0;
        }

        if( delta > 2000 )
        {
            m_TuneFrequency = m_TargetTune;
        }
    }

    void setMode( enum SDR_MODE );
    void setFilter( float bandwidth );

    ComplexSample demodulate( ComplexSample& inputSample );

    double getSampleRate(){return( m_SampleRate ); }

    float  getSignalLevel ();
    float  getSquelchLevel();
    void   setSquelchLevel( float level );

    void   setAGCTime( double upTime, double downTime );

    void   setANF( double value );
    void   setDNR( double value );


    inline void correctIQImbalance( ComplexSample& sample,
                                    float IQPhaseCorrection,
                                    float IQAmplitudeCorrection)
    {
        sample.setI( sample.getI() + IQPhaseCorrection * sample.getQ() );
        sample.setQ( sample.getQ() * (1.0+IQAmplitudeCorrection) );
    }

    inline void correctIQImbalance( ComplexSample& sample )
    {
        correctIQImbalance( sample,
                            m_IQPhaseCorrection,
                            m_IQAmplitudeCorrection );
    }

    inline void estimateIQImbalanceParameters( ComplexSample& sample )
    {
        // update Pointer into Buffers
        m_IQBufferPtr = (m_IQBufferPtr+1)%512;
        static int cnt = 0;
        cnt = (cnt+1)%((int)m_SampleRate/(512*8));

        // update Buffers
        m_IQBuffer0[m_IQBufferPtr] = sample;
        m_IQBuffer1[m_IQBufferPtr] = sample;
        m_IQBuffer2[m_IQBufferPtr] = sample;

        if( m_IQBufferPtr == 0 && cnt == 0 ) // if buffer(s) filled, process...
        {
            // generate new candidate-values
            float testPhase0 = m_IQPhaseCorrection +
                          randomFloat()*M_PI*0.005;
            float testAmplitude0 = m_IQAmplitudeCorrection +
                          randomFloat()*0.00001;

            float testPhase1 = m_IQPhaseCorrection +
                          randomFloat()*M_PI*0.005;
            float testAmplitude1 = m_IQAmplitudeCorrection +
                          randomFloat()*0.00001;

            // correct one buffer with current correction-parameters
            // and correct the other(s) with the new test-parameters
            for( int n=0; n<512; ++n )
            {
                correctIQImbalance( m_IQBuffer0[n] );
                correctIQImbalance( m_IQBuffer1[n], testPhase0, testAmplitude0 );
                correctIQImbalance( m_IQBuffer2[n], testPhase1, testAmplitude1 );
            }

            // weight the result and chose the parameter-set with the better one
            kiss_fft( m_FFT, (kiss_fft_cpx*)m_IQBuffer0 , (kiss_fft_cpx*)m_IQBuffer3 );
            kiss_fft( m_FFT, (kiss_fft_cpx*)m_IQBuffer1 , (kiss_fft_cpx*)m_IQBuffer4 );
            kiss_fft( m_FFT, (kiss_fft_cpx*)m_IQBuffer2 , (kiss_fft_cpx*)m_IQBuffer5 );

            float weight0 = 0;
            float weight1 = 0;
            float weight2 = 0;

            for( int n=32; n<255; ++n )
            {
                float v1 = fabs(m_IQBuffer3[(512+n)%512]);
                float v2 = fabs(m_IQBuffer3[(512-n)%512]);

                float v3 = fabs(m_IQBuffer4[(512+n)%512]);
                float v4 = fabs(m_IQBuffer4[(512-n)%512]);

                float v5 = fabs(m_IQBuffer5[(512+n)%512]);
                float v6 = fabs(m_IQBuffer5[(512-n)%512]);

                weight0 += (v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2)*(v1-v2);
                weight1 += (v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4)*(v3-v4);
                weight2 += (v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6)*(v5-v6);
            }

            if( weight1 > weight0 && weight1 > weight2 )
            {
                m_IQPhaseCorrection = (m_IQPhaseCorrection*9.f+testPhase0)/10.f;
                m_IQAmplitudeCorrection = (m_IQAmplitudeCorrection*3.f+testAmplitude0)/4.f;
            }
            else
            if( weight2 > weight0 && weight2 > weight1 )
            {
                m_IQPhaseCorrection = (m_IQPhaseCorrection*9.f+testPhase1)/10.f;
                m_IQAmplitudeCorrection = (m_IQAmplitudeCorrection*3.f+testAmplitude1)/4.f;
            }
        }
    }

    inline float randomFloat()
    {
        return( (float)rand() / (float)RAND_MAX * 2.f - 1.f );
    }

    private:
    SDRAudio();
    bool update( ComplexSample& input, ComplexSample& output );

    void updateSignalLevel( ComplexSample& input );
    //void updateSquelch    ( ComplexSample& input );
    inline void updateSquelch( ComplexSample& input )
    {
        if( m_SignalLevel <= m_SquelchLevel &&
            m_SquelchCount == 0 )
        {
            // mute it
            input *= 0.f;
        }
        else
        if( m_SignalLevel <= m_SquelchLevel &&
            m_SquelchCount > 0 )
        {
            // just count down and wait
            m_SquelchCount--;
        }
        else
        {
            switch( m_SDRMode )
            {
                case SDR_MODE_FM:
                    // for FM use a fast squelch with a 1/100th of a second
                    m_SquelchCount = (int)(m_SampleRate/100.0);
                    break;
                default:
                    // set delay-counter to 1/4th of a second for all other modes
                    m_SquelchCount = (int)(m_SampleRate/4.0);
                    break;
            }
        }
    }

    enum SDR_MODE   m_SDRMode;

    double          m_SampleRate;

    double          m_Time;
    double          m_TuneFrequency;
    double          m_TargetTune;
    double          m_TuneIncrement;

    FirFilter*      m_FirFilter0;
    FirFilter*      m_FirFilter1;

    volatile double         m_Phase0;
    volatile double         m_Phase1;

    volatile double          m_FilterFrequency;
    volatile double          m_FilterBandwidth;
    volatile double          m_FilterToneShift;

    float           m_SignalLevel;
    float           m_SquelchLevel;
    int             m_SquelchCount;

    AutomaticGainControl* m_AutomaticGainControl;

    float           m_IQPhaseCorrection;
    float           m_IQAmplitudeCorrection;

    ComplexSample   m_IQBuffer0[512];
    ComplexSample   m_IQBuffer1[512];
    ComplexSample   m_IQBuffer2[512];
    ComplexSample   m_IQBuffer3[512];
    ComplexSample   m_IQBuffer4[512];
    ComplexSample   m_IQBuffer5[512];
    int             m_IQBufferPtr;

    kiss_fft_cfg    m_FFT;

    std::fstream    m_SDRLog;
};

#endif
