# ============================================================================
# FindPortaudio.cmake
#
# Copyright 2009,2010,2011 Stefan M. Fendt
# Licensed under LGPL v2.0 or later
#
# ============================================================================

#
# Check if already found or setup externaly
#
IF ( PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS )

  SET( PORTAUDIO_FOUND TRUE )
  MESSAGE( STATUS "Check for libportaudio  --- FOUND." )

#
# if not, try to find it
#
ELSE ( PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS )

  #
  # at first search the required header
  #
  FIND_PATH( PORTAUDIO_INCLUDE_DIR
    NAMES
      portaudio.h

    HINTS
      ENV PATH
      ENV CMAKE_LIBRARY_PATH

    PATHS
      # search pathes for Unix/Linux
      /usr
      /usr/local
      /opt/local

    PATH_SUFFIXES
      ../include
      ../include/portaudio
      /include
      /include/portaudio
  )

  #
  # now try to find the .so/.lib/.dll
  #
  FIND_LIBRARY(PORTAUDIO_LIBRARY
    NAMES
      portaudio

    HINTS
      ENV PATH
      ENV CMAKE_LIBRARY_PATH

    PATHS
      # search pathes for Unix/Linux
      /usr
      /usr/local
      /opt/local

    PATH_SUFFIXES
      ../lib
      ../lib/portaudio
      /lib
      /lib/portaudio
  )

  #
  # copy the result to the output variables of the module
  #
  SET(PORTAUDIO_INCLUDE_DIRS "${PORTAUDIO_INCLUDE_DIR}" )
  SET(PORTAUDIO_LIBRARIES    "${PORTAUDIO_LIBRARY}"     )

  #
  # if these do not end with "-NOT_FOUND", set PORTAUDIO_FOUND to true
  #
  IF (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
     SET(PORTAUDIO_FOUND TRUE)
  ENDIF (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)

  #
  # issue status and if marked as required, puke on not found library
  #
  IF (PORTAUDIO_FOUND)
    MESSAGE(STATUS "Check for libportaudio  --- FOUND.")
  ELSE (PORTAUDIO_FOUND)
    IF (Portaudio_FIND_REQUIRED)
      #
      # be a little more verbose here, as I don't want to leave anyone in the
      # rain... it took me ages to find this in the documentation...
      #
      MESSAGE(FATAL_ERROR
              " =================================================\n"
              " Check for libportadio - not found (but required!)\n"
              " =================================================\n\n"
              " ### Remark: \n"
              " ### \n"
              " ### Under Windows, you might want to set, update or append\n"
              " ### the missing pathes to the following Environment-Variables\n"
              " ### CMAKE_INCLUDE_PATH and CMAKE_LIBRARY_PATH if you are sure\n"
              " ### to have libportaudio installed on your system.\n" )
    ELSEIF (Portaudio_FIND_REQUIRED)
      MESSAGE(STATUS "Check for libportaudio  --- NOT FOUND.")
    ENDIF (Portaudio_FIND_REQUIRED)
  ENDIF (PORTAUDIO_FOUND)

  #
  # store result in cache ...
  #
  MARK_AS_ADVANCED(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

ENDIF (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)

