#include <iostream>
#include <fstream>

#include "complexsample.hpp"
#include "firfilter.hpp"
#include "agc.hpp"

int main( int argc, char** argv )
{
    ComplexSample iSample;
    ComplexSample oSample;
    FirFilter*  filter = new FirFilter( 12000, 2500, 129 );
    AutomaticGainControl*   agc = new AutomaticGainControl( 12000 );

    std::fstream file("out.raw", std::ios::binary|std::ios::out );

    float gain=1;

    for( int n=0; n < 44100; ++n )
    {
        if( (n%2400)==0 )
        {
            gain = (float)rand()/(float)RAND_MAX;
        }

        iSample = ComplexSample( ((float)rand()/(float)RAND_MAX-0.5f)*2.0f,
                                 ((float)rand()/(float)RAND_MAX-0.5f)*2.0f );

        iSample *= gain;

        oSample = filter->update( iSample );
        oSample = agc->update( oSample );

        file.write( (char*)&oSample, sizeof( oSample ) );
    }

    return( 0 );
}