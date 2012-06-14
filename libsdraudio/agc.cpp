#include "agc.hpp"

AutomaticGainControl::AutomaticGainControl( double samplerate,
                                            double Time )
    :
    m_SampleRate(samplerate),
    m_Gain(10.f),
    m_LimitGain(10.f),
    m_Level(1.f),
    m_Level0(1.f),
    m_Level1(1.f),
    m_Level2(1.f),
    m_Level3(1.f),
    m_DataPosition(0)
{
    m_LimitLevel  = powf(10.f,-6.f/20.f);
    m_TargetLevel = powf(10.f,-6.f/20.f);

    m_DataLength = (int)(m_SampleRate/5.0);
    m_Data = new ComplexSample[ m_DataLength ];

    setTime( Time );
}

AutomaticGainControl::AutomaticGainControl()
{
    delete [] m_Data;
}

#include <iostream>
void AutomaticGainControl::setTime( double time )
{
    std::cerr << "AGC-Time:" << time << std::endl;
    std::cerr << "AGC-SampleRate:" << m_SampleRate << std::endl;

    double nrOfSamplesUp   = (double)m_SampleRate * time;
    double nrOfSamplesDown = (double)m_SampleRate * (time/2.0);
    std::cerr << "AGC-Samples:" << nrOfSamplesUp << std::endl;

    // +12/-12 dB in specified Time
    m_GainUp   =     pow( 4.00, 1.0/nrOfSamplesUp   );
    m_GainDown = 1.0/pow( 4.00, 1.0/nrOfSamplesDown );

    std::cerr << "AGC-GainUp...:" << m_GainUp << std::endl;
    std::cerr << "AGC-GainDown.:" << m_GainUp << std::endl;
}

ComplexSample AutomaticGainControl::update( ComplexSample& input )
{
    m_DataPosition = (m_DataPosition+1)%m_DataLength;
    m_Data[m_DataPosition] = input;

    float const currLevel = fabs( input );

    int readPosition = (m_DataPosition+m_DataLength/2)%m_DataLength;

    // reset S&H-Levels at appropriate times
    if( m_DataPosition == (m_DataLength*0/4) )
    {
        m_Level0 = currLevel;
    }
    else
    if( m_DataPosition == (m_DataLength*1/4) )
    {
        m_Level1 = currLevel;
    }
    else
    if( m_DataPosition == (m_DataLength*2/4) )
    {
        m_Level2 = currLevel;
    }
    else
    if( m_DataPosition == (m_DataLength*3/4) )
    {
        m_Level3 = currLevel;
    }

    m_Level0 = ( m_Level0 < currLevel )? currLevel:m_Level0;
    m_Level1 = ( m_Level1 < currLevel )? currLevel:m_Level1;
    m_Level2 = ( m_Level2 < currLevel )? currLevel:m_Level2;
    m_Level3 = ( m_Level3 < currLevel )? currLevel:m_Level3;

    if( (m_DataPosition%(m_DataLength/4)) == 0 )
    {
        m_LastLevel = m_ThisLevel;
        m_ThisLevel = 0.f;
        m_ThisLevel = ( m_ThisLevel < m_Level0 )? m_Level0 : m_ThisLevel;
        m_ThisLevel = ( m_ThisLevel < m_Level1 )? m_Level1 : m_ThisLevel;
        m_ThisLevel = ( m_ThisLevel < m_Level2 )? m_Level2 : m_ThisLevel;
        m_ThisLevel = ( m_ThisLevel < m_Level3 )? m_Level3 : m_ThisLevel;
    }

    float weight = (float)(m_DataPosition%(m_DataLength/4))/
                   (float)(m_DataLength/4);

    m_Level = (cosf(M_PI*(    weight))*0.5f+0.5f)*m_LastLevel+
              (cosf(M_PI*(1.f-weight))*0.5f+0.5f)*m_ThisLevel;

    if( m_Level != 0.f )
    {
        m_LimitGain = m_LimitLevel / m_Level;
    }

    m_LimitGain = (m_LimitGain <        0.01f)?        0.01f:m_LimitGain;
    m_LimitGain = (m_LimitGain > 100000000.0f)? 100000000.0f:m_LimitGain;

    float const targetGain = (m_TargetLevel/m_Level);

    if( m_Gain < targetGain )
    {
        m_Gain *= m_GainUp;
    }
    else
    if( m_Gain > targetGain )
    {
        m_Gain *= m_GainDown;
    }

    m_Gain = (m_Gain <        0.1f)?        0.1f:m_Gain;
    m_Gain = (m_Gain > 10000000.0f)? 10000000.0f:m_Gain;

    float const gain = (float)(( m_Gain <= m_LimitGain )? m_Gain:m_LimitGain);

    ComplexSample out( m_Data[readPosition]*gain );

    return( out );
}
