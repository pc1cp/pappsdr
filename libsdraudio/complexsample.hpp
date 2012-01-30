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

#ifndef __COMPLEXSAMPLE_HPP__
#define __COMPLEXSAMPLE_HPP__

#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926f
#endif

class ComplexSample
{
    public:

    // ========================================================================
    // some possible constructors
    // ========================================================================

    ComplexSample();
    ComplexSample( float real );
    ComplexSample( float I, float Q );

    // ========================================================================
    // get/set components
    // ========================================================================

    float getI();
    float getQ();
    void  setI( float I );
    void  setQ( float Q );

    inline char* getByteAddress();

    // ========================================================================
    // operators which modify us directly .. As we are required to calculate
    // some fancy stuff with these samples, it might be a good idea to have the
    // basic math-operators defined. It will greatly enhance code-readability.
    //
    // BTW: Do we really need a division for this? Hmm,...
    // ========================================================================

    inline ComplexSample& operator  = ( const ComplexSample& other );
    inline ComplexSample& operator  = ( const float&         other );
    inline ComplexSample& operator += ( const ComplexSample& other );
    inline ComplexSample& operator += ( const float&         other );
    inline ComplexSample& operator -= ( const ComplexSample& other );
    inline ComplexSample& operator -= ( const float&         other );
    inline ComplexSample& operator *= ( const ComplexSample& other );
    inline ComplexSample& operator *= ( const float& other );
    inline ComplexSample& operator /= ( const ComplexSample& other );
    inline ComplexSample& operator /= ( const float&         other );

    private:

    union
    {
        char    m_ByteData[8];
        struct
        {
            float   m_I;
            float   m_Q;
        };
    };
};

// ============================================================================
// Definition of the constructors
// ============================================================================

inline ComplexSample::ComplexSample()
{
    m_I = 0.f;
    m_Q = 0.f;
}

inline ComplexSample::ComplexSample( float real )
{
    m_I = real;
    m_Q = 0.f;
}

inline ComplexSample::ComplexSample( float I, float Q )
{
    m_I = I;
    m_Q = Q;
}

// ============================================================================
// Definition of getters and setters
// ============================================================================

inline float ComplexSample::getI()
{
    return( m_I );
}

inline float ComplexSample::getQ()
{
    return( m_Q );
}

inline void  ComplexSample::setI( float I )
{
    m_I = I;
}

inline void  ComplexSample::setQ( float Q )
{
    m_Q = Q;
}

inline char* ComplexSample::getByteAddress()
{
    return( &m_ByteData[0] );
}

// ============================================================================
// handle addition operator for different cases ...
// ============================================================================

inline ComplexSample& ComplexSample::operator  = ( const ComplexSample& other )
{
    m_I = other.m_I;
    m_Q = other.m_Q;
    return( *this );
}

inline ComplexSample& ComplexSample::operator  = ( const float&         other )
{
    m_I = other;
    m_Q = 0.0f;
    return( *this );
}

inline ComplexSample& ComplexSample::operator += ( const ComplexSample& other )
{
    m_I += other.m_I;
    m_Q += other.m_Q;
    return( *this );
}

inline ComplexSample& ComplexSample::operator += ( const float&         other )
{
    m_I += other;
    return( *this );
}

inline ComplexSample& ComplexSample::operator -= ( const ComplexSample& other )
{
    m_I -= other.m_I;
    m_Q -= other.m_Q;
    return( *this );
}

inline ComplexSample& ComplexSample::operator -= ( const float&         other )
{
    m_I -= other;
    return( *this );
}

inline ComplexSample& ComplexSample::operator *= ( const ComplexSample& other )
{
    ComplexSample newSample;

    newSample.setI( m_I * other.m_I -
                    m_Q * other.m_Q );

    newSample.setQ( m_I * other.m_Q +
                    m_Q * other.m_I );

    m_I = newSample.m_I;
    m_Q = newSample.m_Q;

    return( *this );
}

inline ComplexSample& ComplexSample::operator *= ( const float& other )
{
    m_I *= other;
    m_Q *= other;

    return( *this );
}

inline ComplexSample& ComplexSample::operator /= ( const ComplexSample& other )
{
    float const a = m_I;
    float const b = m_Q;
    float const c = other.m_I;
    float const d = other.m_Q;
    m_I = ( a*c + b*d ) / ( c*c + d*d );
    m_Q = ( b*c - a*d ) / ( c*c + d*d );
    return( *this );
}

inline ComplexSample& ComplexSample::operator /= ( const float&         other )
{
    float const a = m_I;
    float const b = m_Q;
    float const c = other;
    float const d = 0.f;
    m_I = ( a*c + b*d ) / ( c*c + d*d );
    m_Q = ( b*c - a*d ) / ( c*c + d*d );
    return( *this );
}

inline
ComplexSample operator + (const ComplexSample& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = a;
    c += b;
    return( c );
}

inline
ComplexSample operator + (const ComplexSample& a, const float& b )
{
    ComplexSample c;
    c  = a;
    c += b;
    return( c );
}

inline
ComplexSample operator + (const float& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = b;
    c += a;
    return( c );
}

// ============================================================================
// handle substraction operator for different cases ...
// ============================================================================

inline
ComplexSample operator - (const ComplexSample& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = a;
    c -= b;
    return( c );
}

inline
ComplexSample operator - (const ComplexSample& a, const float& b )
{
    ComplexSample c;
    c  = a;
    c -= b;
    return( c );
}

inline
ComplexSample operator - (const float& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = b;
    c -= a;
    return( c );
}

// ============================================================================
// handle multiplication operator for different cases ...
// ============================================================================

inline
ComplexSample operator * (const ComplexSample& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = a;
    c *= b;
    return( c );
}

inline
ComplexSample operator * (const ComplexSample& a, const float& b )
{
    ComplexSample c;
    c  = a;
    c *= b;
    return( c );
}

inline
ComplexSample operator * (const float& a, const ComplexSample& b )
{
    ComplexSample c;
    c  = b;
    c *= a;
    return( c );
}

// ============================================================================
// having an overloaded fabs is handy, too...
// ============================================================================

inline
float fabs( ComplexSample& sample )
{
    return( sqrtf( sample.getI()*sample.getI()+sample.getQ()*sample.getQ() ) );
}

#endif
