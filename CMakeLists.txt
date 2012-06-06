# ==============================================================================
# Build PappSDR Controll-Interface
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( PAPPSDR )

# update current module path, so we have a FindPortaudio.cmake at hand
SET( CMAKE_MODULE_PATH
     ${CMAKE_MODULE_PATH}
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )


IF(UNIX)
    ADD_DEFINITIONS( #-Wall
                     #-Wextra
                     #-Werror
                     -O3
                     #-g
                     #-pg
                     -march=native
                     -msse2
                     -mfpmath=sse,387
                     -ffast-math
                     )
ENDIF(UNIX)

IF( MSVC )
	# for the Microsoft-compilers turn off some misleading and wrong(!)
	# warnings about standard-library-code, which are intended to urge you
	# to produce code which is compatible with Microsoft Windows, only...
	SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")

	# we most probably will use exception-handling in c++ ... why ever this
	# is not fully supported without this flag ... will remain a mystery only
	# known to M$...
	SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /EHsc")
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

	# to turn off Microsoft-Compiler-Extensions use this...
	#SET( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /Za")
	#SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Za")

	# ==============================================================================
	# Setup output directories
	# ==============================================================================

	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY         ${PAPPSDR_BINARY_DIR}/bin )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${PAPPSDR_BINARY_DIR}/bin )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY         ${PAPPSDR_BINARY_DIR}/bin )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PAPPSDR_BINARY_DIR}/Debug )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PAPPSDR_BINARY_DIR}/Debug )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${PAPPSDR_BINARY_DIR}/Debug )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PAPPSDR_BINARY_DIR}/Release )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PAPPSDR_BINARY_DIR}/Release )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PAPPSDR_BINARY_DIR}/Release )

ENDIF( MSVC )

# build the library to acess the hardware
ADD_SUBDIRECTORY   ( libpappradio )
INCLUDE_DIRECTORIES( libpappradio/source/libpappradio/ )

ADD_SUBDIRECTORY   ( libsdraudio )
INCLUDE_DIRECTORIES( libsdraudio )

ADD_SUBDIRECTORY   ( libkissfft )
INCLUDE_DIRECTORIES( libkissfft )

# build the simple sdr-receiver-software if possible
FIND_PACKAGE( wxWidgets COMPONENTS core base net )
FIND_PACKAGE( Portaudio )

IF(wxWidgets_FOUND)
    IF(PORTAUDIO_FOUND)
        INCLUDE(${wxWidgets_USE_FILE})
        INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIRS} )
        INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
        ADD_SUBDIRECTORY( source )
    ELSE()
        MESSAGE( "Libportaudio not found! Will not build frontend!")
    ENDIF()
ELSE()
    MESSAGE( "WxWidgets not found! Will not build frontend!")
ENDIF()


# ==============================================================================
# Try to generate a deb-package for this...

#INCLUDE (InstallRequiredSystemLibraries)
#
#SET (CPACK_SET_DESTDIR "on")
#SET (CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
#SET (CPACK_GENERATOR "DEB")
#
#SET (VERSION 0.0.3)
#SET (CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
#SET (${VERSION} CPACK_DEBIAN_PACKAGE_VERSION)
#SET (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
#SET (CPACK_DEBIAN_PACKAGE_SECTION "HAM radio")
#SET (CPACK_DEBIAN_ARCHITECTURE $ {CMAKE_SYSTEM_PROCESSOR})
#SET (CPACK_DEBIAN_PACKAGE_DEPENDS
#     "libc6 (>=2.1.3), libgcc1 (>=1:4.1.1), libportaudio2 (>=19), libwxgtk2.8-0 (>=2.8)" )
#
#SET (CPACK_PACKAGE_DESCRIPTION "Simple frontend for the pappradio SDR-receiver")
#SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "This package contains a simple control-frontend for the pappradio SDR-receiver (http://www.pappradio.de). This is a developers-snapshot, so please keep in mind, that the one ore other feature still may not be working, like you might hope..." )
#SET (CPACK_PACKAGE_CONTACT "Stefan Fendt <sfendt@googlemail.com>")
#SET (CPACK_PACKAGE_LICENSE "GNU Lesser Public License (lGPL V3.0)" )
#SET (CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION}_Ubuntu11.11_Amd64")
#
#SET (CPACK_COMPONENTS_ALL Libraries ApplicationData)
#INCLUDE(CPack)


