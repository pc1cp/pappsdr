# ==============================================================================
# Build PappSDR Controll-Interface
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( PAPPSDR )

# update current module path, so we have a FindPortaudio.cmake at hand
SET( CMAKE_MODULE_PATH
     ${CMAKE_MODULE_PATH}
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

INCLUDE_DIRECTORIES( libpappradio )

IF(UNIX)
    ADD_DEFINITIONS( -Wall
                     -Wextra
                     -Werror
                     -O3
                     -g
                     #-pg
                     -march=native
                     -msse2
                     -mfpmath=sse,387
                     -ffast-math
                     )
ENDIF(UNIX)

# ==============================================================================
# Copy files function
# ==============================================================================

FUNCTION(FILE_COPY target to_dir)
    FILE(MAKE_DIRECTORY "${to_dir}")
    SET(targets)
    FOREACH(file ${ARGN})
        GET_FILENAME_COMPONENT(file_name ${file} NAME)
        IF("${to_dir}" STREQUAL "")
            SET(to ${file_name})
        ELSE()
            SET(to "${to_dir}/${file_name}")
        ENDIF()
        ADD_CUSTOM_COMMAND(OUTPUT ${to}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} ${to}
        COMMAND ${CMAKE_COMMAND} -E touch_nocreate "${to}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        MAIN_DEPENDENCY ${file}
        COMMENT "Copy ${file} to ${to}")
        SET(targets ${targets} ${to})
    ENDFOREACH()
    ADD_CUSTOM_TARGET(${target} DEPENDS ${targets})
ENDFUNCTION()

# ==============================================================================
# Setup output directories
# ==============================================================================

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all libraries" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all libraries" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all libraries" )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
     ${PAPPRADIO_BINARY_DIR}/bin
     CACHE PATH
     "Single directory for all executables" )

# ==============================================================================

# build the library to acess the hardware
ADD_SUBDIRECTORY( libpappradio )

ADD_SUBDIRECTORY( libsdraudio )
INCLUDE_DIRECTORIES( libsdraudio )

ADD_SUBDIRECTORY( libkissfft )
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
    ENDIF()
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


