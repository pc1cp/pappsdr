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

    void setTune( float frequency ){ m_TuneFrequency = frequency; }
    void setMode( enum SDR_MODE );
    void setFilter( float bandwidth );

    ComplexSample demodulate( ComplexSample& inputSample );

    double getSampleRate(){return( m_SampleRate ); }

    float  getSignalLevel ();
    float  getSquelchLevel();
    void   setSquelchLevel( float level );

    void   setAGCTime( double upTime, double downTime );

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
                    m_SquelchCount = m_SampleRate/100.f;
                    break;
                default:
                    // set delay-counter to 1/4th of a second for all other modes
                    m_SquelchCount = m_SampleRate/4.f;
                    break;
            }
        }
    }


    enum SDR_MODE   m_SDRMode;

    double          m_SampleRate;
    //double          m_CoreSampleRate;

    double          m_Time;
    double          m_TuneFrequency;

    int             m_Cascades;
    //Downsampler**   m_Downsamplers;

    FirFilter*      m_FirFilter0;
    FirFilter*      m_FirFilter1;

    double          m_FilterFrequency;
    double          m_FilterBandwidth;
    double          m_FilterToneShift;

    float           m_SignalLevel;
    float           m_SquelchLevel;
    int             m_SquelchCount;

    AutomaticGainControl* m_AutomaticGainControl;
};

#endif
