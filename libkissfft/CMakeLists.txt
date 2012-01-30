# ==============================================================================
# Build libkissfft as a static library
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( LIBKISSFFT )

# set the sources and headers(!) so if using an IDE (like codeblocks or VSC++)
# we can see both...
SET( SOURCES    kissfft.hpp
                kiss_fft.h
                kiss_fft.c
                _kiss_fft_guts.h )

# we would like to build a shared library out of our source-files
ADD_LIBRARY( kissfft ${SOURCES} )

#INSTALL( INSTALL PappSDR DESTINATION /usr/local/bin/ )
