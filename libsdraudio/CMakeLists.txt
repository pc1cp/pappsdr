# ==============================================================================
# Build libsdraudio as a shared library
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( LIBSDRAUDIO )

INCLUDE_DIRECTORIES( ../libkissfft/ )

# set the sources and headers(!) so if using an IDE (like codeblocks or VSC++)
# we can see both...

SET( HEADERS    libsdraudio.hpp
                complexsample.hpp
                agc.hpp
                firfilter.hpp
                )

SET( SOURCES    libsdraudio.cpp
                agc.cpp
                firfilter.cpp
                )

# we would like to build a shared library out of our source-files
##ADD_LIBRARY( sdraudio SHARED ${SOURCES} )
ADD_LIBRARY( sdraudio STATIC ${SOURCES} )
#SET_TARGET_PROPERTIES( sdraudio PROPERTIES LINK_FLAGS -pg)


