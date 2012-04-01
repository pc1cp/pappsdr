# ==============================================================================
# Build a simple SDR receiver-software
# ==============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( PAPPRADIOSDR )

SET( SOURCES    main.hpp
                main.cpp
                config.hpp
                config.cpp
                sounddevicedialog.hpp
                sounddevicedialog.cpp
                audioqueue.hpp
                audioqueue.cpp
                audiothread.hpp
                audiothread.cpp
                lcdwidget.hpp
                lcdwidget.cpp
                fftwidget.hpp
                fftwidget.cpp
                pushbutton.hpp
                pushbutton.cpp
                separator.cpp
                separator.hpp
                configdialog.cpp
                configdialog.hpp
				spinctrl.cpp
				spinctrl.hpp )

# add binary resources ...
SET(ARTPATH "${CMAKE_CURRENT_SOURCE_DIR}/../artwork")
SET(GENPATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
FILE( MAKE_DIRECTORY "${GENPATH}" )
ADD_EXECUTABLE( bin2c bin2c.cpp )

SET( RESOURCES "" )
MACRO( ADD_BINARY_RESOURCE FNAME )
    GET_FILENAME_COMPONENT(PNAME ${FNAME} NAME_WE )
    GET_FILENAME_COMPONENT(EXT   ${FNAME} EXT )

    STRING(REPLACE "." "" EXT "${EXT}")

    SET(RNAME "${PNAME}_${EXT}")
    SET(ONAME "${PNAME}_${EXT}.cpp")

    MESSAGE("-- Adding binary resource \"${PNAME}.${EXT}\"." )

    ADD_CUSTOM_COMMAND( OUTPUT "${GENPATH}/${ONAME}"
                        COMMAND bin2c "${FNAME}" "${RNAME}" "${GENPATH}/${ONAME}"
                        DEPENDS bin2c "${FNAME}"
                        COMMENT "Generating source-file \"${ONAME}\"." )
    SET(RESOURCES "${RESOURCES}" "${GENPATH}/${ONAME}")
ENDMACRO( ADD_BINARY_RESOURCE )

ADD_BINARY_RESOURCE( "${ARTPATH}/pushbutton.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/pushbutton_hover.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/pushbutton_active.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/pushbutton_active_hover.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/LCD_background.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/LCD_backgrounddB.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/LCD_foreground.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/LCD_largenumbers.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/LCD_smallnumbers.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/separator.png" )
ADD_BINARY_RESOURCE( "${ARTPATH}/Icon.png" )

# we would like to build a program
ADD_EXECUTABLE( PappSDR WIN32 ${SOURCES} ${RESOURCES} )
TARGET_LINK_LIBRARIES(  PappSDR
                        pappradio
                        sdraudio
                        kissfft
                        ${wxWidgets_LIBRARIES}
                        ${PORTAUDIO_LIBRARIES} )

#SET_TARGET_PROPERTIES( PappSDR PROPERTIES LINK_FLAGS -pg)

INSTALL( TARGETS PappSDR DESTINATION /usr/bin/ )

