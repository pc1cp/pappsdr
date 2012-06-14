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
                     -g
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

	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin )

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
        # =====================================================================
        # OK, we have found both: wxWidgets and portaudio. This allows us to
        # build the graphical front end.
        # =====================================================================

        INCLUDE(${wxWidgets_USE_FILE})
        INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIRS} )
        INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
        ADD_SUBDIRECTORY( source )

        # to make it work from source, however we'd like to copy the portaudio
        # DLL into the build-dir if under windows.
        IF( WIN32 )
            # path is the same as for PORTAUIO_LIBRARY except that it's not
            # portaudio_x86.lib we look for
            STRING( REGEX REPLACE ".lib" ".dll" PORTAUDIO_DLL "${PORTAUDIO_LIBRARY}" )
            MESSAGE( STATUS "USING THIS PORTAUDIO-DLL.: ${PORTAUDIO_DLL}" )
            CONFIGURE_FILE( "${PORTAUDIO_DLL}" "${CMAKE_BINARY_DIR}/bin/" COPYONLY)

            FIND_FILE( MSVC_C_RUNTIME_DLL "SysWOW64/msvcr100.dll" )
            IF( !MSVC_C_RUNTIME_DLL )
                FIND_FILE( MSVC_C_RUNTIME_DLL "msvcr100.dll" )
            ENDIF()

            FIND_FILE( MSVC_CPP_RUNTIME_DLL "SysWOW64/msvcp100.dll" )
            IF( !MSVC_CPP_RUNTIME_DLL )
                FIND_FILE( MSVC_CPP_RUNTIME_DLL "msvcp100.dll" )
            ENDIF()

            MESSAGE( STATUS "USING THIS MSVC-RUNTIME..: ${MSVC_C_RUNTIME_DLL}" )
            MESSAGE( STATUS "USING THIS MSVCpp-RUNTIME: ${MSVC_CPP_RUNTIME_DLL}" )
            CONFIGURE_FILE( "${MSVC_C_RUNTIME_DLL}" "${CMAKE_BINARY_DIR}/bin/" COPYONLY)
            CONFIGURE_FILE( "${MSVC_CPP_RUNTIME_DLL}" "${CMAKE_BINARY_DIR}/bin/" COPYONLY)

        ENDIF( WIN32 )
    ELSE()
        MESSAGE( FATAL "Libportaudio not found! Will not build frontend!")
    ENDIF()
ELSE()
    MESSAGE( FATAL "WxWidgets not found! Will not build frontend!")
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


