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

#include "firfilter.hpp"
#include <iostream>

FirFilter::FirFilter( double  sampleRate, double f0, double f1, int length )
{
    // store sampleRate
    m_SampleRate = sampleRate;

    // find next power of two size for FFT
    double lenMin = pow( 2.0, floor( log( (double)length )/log(2.0) ) );
    double lenMax = pow( 2.0, ceil ( log( (double)length )/log(2.0) ) );
    m_FilterLength = (int)( fabs( lenMin-(double)length ) <
                            fabs( lenMax-(double)length ) ?
                            lenMin:
                            lenMax );

    // get sufficiently sized coefficients array
    m_CoefficientsFFT = new kiss_fft_cpx[ m_FilterLength ];

    // get sufficiently sized buffers
    m_In_0            = new kiss_fft_cpx[ m_FilterLength ];
    m_In_1            = new kiss_fft_cpx[ m_FilterLength ];
    m_Out0            = new kiss_fft_cpx[ m_FilterLength ];
    m_Out1            = new kiss_fft_cpx[ m_FilterLength ];
    m_Out2            = new kiss_fft_cpx[ m_FilterLength ];
    m_Out3            = new kiss_fft_cpx[ m_FilterLength ];
    m_Mean            = new kiss_fft_cpx[ m_FilterLength ];

    // clear buffers
    for( int n=0; n<m_FilterLength; n++ )
    {
        m_In_0[n].r = 0;
        m_In_0[n].i = 0;
        m_In_1[n].r = 0;
        m_In_1[n].i = 0;
        m_Out0[n].r = 0;
        m_Out0[n].i = 0;
        m_Out1[n].r = 0;
        m_Out1[n].i = 0;
        m_Out2[n].r = 0;
        m_Out2[n].i = 0;
        m_Out3[n].r = 0;
        m_Out3[n].i = 0;
        m_Mean[n].r = 0;
        m_Mean[n].i = 0;
    }

    m_KissCFGforeward = kiss_fft_alloc( m_FilterLength, 0, 0, 0 );
    m_KissCFGbackward = kiss_fft_alloc( m_FilterLength, 1, 0, 0 );

    // setup filter-coefficients
    setBandwidth( f0, f1 );

    // setup array-pointers
    m_Pos0 = 0;
    m_Pos1 = m_FilterLength/2;

    // clear DeHumm-Filter-Flag
    m_DeHummSet = false;
}

FirFilter::~FirFilter()
{
    kiss_fft_free( m_KissCFGforeward );
    kiss_fft_free( m_KissCFGbackward );

    delete [] m_CoefficientsFFT;
    delete [] m_In_0;
    delete [] m_In_1;
    delete [] m_Out0;
    delete [] m_Out1;
    delete [] m_Out2;
    delete [] m_Out3;

    delete [] m_Mean;
}

void FirFilter::setDeHumm()
{
    m_DeHummSet = true;
}

void FirFilter::setBandwidth( double fMin, double fMax )
{
    // swap values if specified in wrong order
    if( fMax < fMin )
    {
        double swap = fMax;
        fMax = fMin;
        fMin = swap;
    }

    double fDelta    = m_SampleRate / (double)m_FilterLength;
    double fMinIndex = floor( fMin / fDelta + 0.5 );
    double fMaxIndex = floor( fMax / fDelta + 0.5 );

    for( int n=0; n<(m_FilterLength/2); ++n )
    {
        if( n >= fMinIndex && n <= fMaxIndex )
        {
            m_CoefficientsFFT[n].r = 1.f/(float)m_FilterLength;
            m_CoefficientsFFT[n].i = 1.f/(float)m_FilterLength;
            m_CoefficientsFFT[m_FilterLength-1-n].r = 1.f/(float)m_FilterLength;
            m_CoefficientsFFT[m_FilterLength-1-n].i = 1.f/(float)m_FilterLength;
        }
        else
        {
            m_CoefficientsFFT[n].r = 0.f;
            m_CoefficientsFFT[n].i = 0.f;
            m_CoefficientsFFT[m_FilterLength-1-n].r = 0.f;
            m_CoefficientsFFT[m_FilterLength-1-n].i = 0.f;
        }
    }
}

ComplexSample FirFilter::update( ComplexSample input )
{
    m_Pos0=(m_Pos0+1)%m_FilterLength;
    m_Pos1=(m_Pos0+m_FilterLength/2)%m_FilterLength;

    m_In_0[m_Pos0].r = input.getI();
    m_In_0[m_Pos0].i = input.getQ();
    m_In_1[m_Pos1].r = input.getI();
    m_In_1[m_Pos1].i = input.getQ();

    if( m_Pos0 == 0 )
    {
        kiss_fft( m_KissCFGforeward, m_In_0 , m_Out0 );
        for( int n=0; n<m_FilterLength; n++ )
        {
            m_Out0[n].r *= m_CoefficientsFFT[n].r;
            m_Out0[n].i *= m_CoefficientsFFT[n].i;
#if 1
            if( m_DeHummSet )
            {
                float amplitude = sqrtf( m_Out0[n].r * m_Out0[n].r + m_Out0[n].i * m_Out0[n].i );
                m_Mean[n].r = m_Mean[n].r * 0.99 + 0.01 * amplitude;
                m_Mean[n].i = atan2f( m_Out0[n].r , m_Out0[n].i );

                //m_Out0[n].r = m_Mean[n].r*sinf(m_Mean[n].i);
                //m_Out0[n].i = m_Mean[n].r*cosf(m_Mean[n].i);
            }
#endif
        }

        if( m_DeHummSet )
        {
            float mean = 0.f;
            float max  = 0.f;

            for( int n=0; n<m_FilterLength; n++ )
            {
                mean += m_Mean[n].r;
                max   = max < m_Mean[n].r;
            }

            mean /= (float)m_FilterLength;
            float contrast = 0.333f+(max+mean)/(max-mean);

            for( int n=0; n<m_FilterLength; n++ )
            {
                float amplitude = sqrtf( m_Out0[n].r * m_Out0[n].r + m_Out0[n].i * m_Out0[n].i );

                amplitude -= m_Mean[n  ].r*0.25;

                amplitude  = (amplitude < 0.f)? 0.f:amplitude;

                m_Out0[n].r = amplitude*sinf(m_Mean[n].i);
                m_Out0[n].i = amplitude*cosf(m_Mean[n].i);

                if(1)
                if( (m_Mean[n].r/1.5f) > m_Mean[(n-3)%m_FilterLength].r &&
                    (m_Mean[n].r/1.5f) > m_Mean[(n+3)%m_FilterLength].r )//&&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-4)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+4)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-5)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+5)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-6)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+6)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-7)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+7)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-8)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+8)%m_FilterLength].r )
                {
                    m_Out0[n-2].r = 0.f;
                    m_Out0[n-2].i = 0.f;
                    m_Out0[n-1].r = 0.f;
                    m_Out0[n-1].i = 0.f;
                    m_Out0[n  ].r = 0.f;
                    m_Out0[n  ].i = 0.f;
                    m_Out0[n+1].r = 0.f;
                    m_Out0[n+1].i = 0.f;
                    m_Out0[n+2].r = 0.f;
                    m_Out0[n+2].i = 0.f;
                }
            }
        }

        kiss_fft( m_KissCFGbackward, m_Out0, m_Out1  );
    }

    if( m_Pos1 == 0 )
    {
        kiss_fft( m_KissCFGforeward, m_In_1 , m_Out2 );
        for( int n=0; n<m_FilterLength; n++ )
        {
            m_Out2[n].r *= m_CoefficientsFFT[n].r;
            m_Out2[n].i *= m_CoefficientsFFT[n].i;

#if 1
            if( m_DeHummSet )
            {
                float amplitude = sqrtf( m_Out2[n].r * m_Out2[n].r + m_Out2[n].i * m_Out2[n].i );
                m_Mean[n].r = m_Mean[n].r * 0.99 + 0.01 * amplitude;
                m_Mean[n].i = atan2f( m_Out2[n].r , m_Out2[n].i );

                //m_Out2[n].r = m_Mean[n].r*sinf(m_Mean[n].i);
                //m_Out2[n].i = m_Mean[n].r*cosf(m_Mean[n].i);
            }
#endif
        }

        if( m_DeHummSet )
        {
            float mean = 0.f;
            float max  = 0.f;

            for( int n=0; n<m_FilterLength; n++ )
            {
                mean += m_Mean[n].r;
                max   = max < m_Mean[n].r;
            }

            mean /= (float)m_FilterLength;
            float contrast = 0.333f+(max+mean)/(max-mean);

            for( int n=0; n<m_FilterLength; n++ )
            {
                float amplitude = sqrtf( m_Out2[n].r * m_Out2[n].r + m_Out2[n].i * m_Out2[n].i );

                amplitude -= m_Mean[n  ].r*0.25;

                amplitude  = (amplitude < 0.f)? 0.f:amplitude;

                m_Out2[n].r = amplitude*sinf(m_Mean[n].i);
                m_Out2[n].i = amplitude*cosf(m_Mean[n].i);

                if(1)
                if( (m_Mean[n].r/1.5f) > m_Mean[(n-3)%m_FilterLength].r &&
                    (m_Mean[n].r/1.5f) > m_Mean[(n+3)%m_FilterLength].r )//&&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-4)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+4)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-5)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+5)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-6)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+6)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-7)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+7)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n-8)%m_FilterLength].r &&
                    //(m_Mean[n].r/1.5f) > m_Mean[(n+8)%m_FilterLength].r )
                {
                    m_Out2[n-2].r = 0.f;
                    m_Out2[n-2].i = 0.f;
                    m_Out2[n-1].r = 0.f;
                    m_Out2[n-1].i = 0.f;
                    m_Out2[n  ].r = 0.f;
                    m_Out2[n  ].i = 0.f;
                    m_Out2[n+1].r = 0.f;
                    m_Out2[n+1].i = 0.f;
                    m_Out2[n+2].r = 0.f;
                    m_Out2[n+2].i = 0.f;
                }
            }
        }

        kiss_fft( m_KissCFGbackward, m_Out2, m_Out3  );
    }

    float w0 = sin((float)m_Pos0/(float)m_FilterLength*M_PI); w0 *= w0;
    float w1 = 1.0-w0;

    ComplexSample output0( m_Out1[m_Pos0].r, m_Out1[m_Pos0].i );
    output0 *= w0;
    ComplexSample output1( m_Out3[m_Pos1].r, m_Out3[m_Pos1].i );
    output1 *= w1;

    return(output0+output1);
}

