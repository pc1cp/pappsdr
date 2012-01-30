// ============================================================================
//
// File .......: sample.hpp
//
// Author(s) ..: Stefan Fendt
//
// Licence ....: GNU-GPL V2.0 or later
//
// Purpose ....: Declaration of a stereo-sample for the Internet-Radio-Client
//               as well as some operations you can do on them ...
//
// ============================================================================

#ifndef __SAMPLE_HPP__
#define __SAMPLE_HPP__

class Sample_t
{
    public:

    float l;
    float r;

    Sample_t( float left=0, float right=0 )
    {
        l=left;
        r=right;
    }

    Sample_t operator + ( Sample_t sample2 )
    {
       l += sample2.l;
       r += sample2.r;
       return( *this );
    }

    Sample_t operator += ( Sample_t sample2 )
    {
       l += sample2.l;
       r += sample2.r;
       return( *this );
    }

    Sample_t operator * ( Sample_t sample2 )
    {
       l *= sample2.l;
       r *= sample2.r;
       return( *this );
    }

    Sample_t operator * ( float value )
    {
       l *= value;
       r *= value;
       return( *this );
    }

    Sample_t operator = ( Sample_t sample2 )
    {
       l = sample2.l;
       r = sample2.r;
       return( *this );
    }
};

#endif
