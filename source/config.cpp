
#include "config.hpp"
#include "sounddevicedialog.hpp"
#include "configdialog.hpp"

#include <ciso646>

BEGIN_EVENT_TABLE(GlobalLogging, wxFrame)
    EVT_TIMER  ( 0, GlobalLogging::onTimer )
    EVT_CLOSE  ( GlobalLogging::onClose )
END_EVENT_TABLE()

GlobalConfig::GlobalConfig()
{
    m_LoggingWindow = new GlobalLogging;
    m_AudioThread = 0;

    // initializing portaudio
    PaError err = Pa_Initialize();

    if( err != paNoError )
    {
        Log( "Error initializing Portaudio: %s", Pa_GetErrorText( err ) );
        exit(-1);
    }

    // get available soundcards from portaudio
    int nDevices;

    nDevices = Pa_GetDeviceCount();
    if( nDevices < 0 )
    {
        Log( "Error, Pa_CountDevices returned : %i", nDevices );
        Log( "Error initializing Portaudio: %s", Pa_GetErrorText( nDevices ));
        exit(-1);
    }
    else
    {
        // Ok, so we got some audio-devices, it's time to test if they support
        // what we need...

        const PaDeviceInfo* DeviceInfo;

        for( int i=0; i<nDevices; i++ )
        {
            DeviceInfo = Pa_GetDeviceInfo( i );
			Log( "Device-Index: %i", i );
			Log( "Device-Name: %s", DeviceInfo->name );
			Log( "Device-Max-Input-Channels: %i", DeviceInfo->maxInputChannels );
			Log( "Device-Max-Output-Channels: %i", DeviceInfo->maxOutputChannels );
			const PaHostApiInfo* HostApiInfo = Pa_GetHostApiInfo( DeviceInfo->hostApi );
			Log( "Device-Host-API: %s", HostApiInfo->name );
			Log( " " );
		}

        // Build up list of usable output devices...
        for( int i=0; i<nDevices; i++ )
        {
            DeviceInfo = Pa_GetDeviceInfo( i );
			const PaHostApiInfo* HostApiInfo = Pa_GetHostApiInfo( DeviceInfo->hostApi );

#ifndef _WIN32
            if( DeviceInfo->maxOutputChannels >= 2 )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }
#else
            if( DeviceInfo->maxOutputChannels >= 2 &&
				HostApiInfo->type == paWDMKS      )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[WDMKS] - ")+wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }

            if( DeviceInfo->maxOutputChannels >= 2 &&
				HostApiInfo->type == paWASAPI      )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[WASAPI] - ")+wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }

            if( DeviceInfo->maxOutputChannels >= 2 &&
				HostApiInfo->type == paMME         )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[MME] - ")+wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }

            if( DeviceInfo->maxOutputChannels >= 2 &&
				HostApiInfo->type == paDirectSound  )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[DirectSound] - ")+wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }
#endif
        }

        // Build up list of usable input devices...
        for( int i=0; i<nDevices; i++ )
        {
            DeviceInfo = Pa_GetDeviceInfo( i );
			const PaHostApiInfo* HostApiInfo = Pa_GetHostApiInfo( DeviceInfo->hostApi );

#ifndef _WIN32
            if( DeviceInfo->maxInputChannels >= 2 )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }
#else
            if( DeviceInfo->maxInputChannels >= 2 &&
				HostApiInfo->type == paWDMKS      )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[WDMKS] - ") + wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }

            if( DeviceInfo->maxInputChannels >= 2 &&
				HostApiInfo->type == paWASAPI     )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[WASAPI] - ") + wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }

            if( DeviceInfo->maxInputChannels >= 2 &&
				HostApiInfo->type == paMME      )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[MME] - ") + wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }

            if( DeviceInfo->maxInputChannels >= 2 &&
				HostApiInfo->type == paDirectSound )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = _("[DirectSound] - ") + wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }
#endif
        }
    }

    // now try to find out which audio-device was stored the last time
    m_OutputDeviceIndex =  0;
    m_InputDeviceIndex  =  0;
    wxString lastInputDevice = m_Registry.getInputDevice();
    wxString lastOutputDevice = m_Registry.getOutputDevice();

    // and if it's in the current device-list, then choose it again
    for( unsigned int i=0; i<m_AudioInputDevices.size(); i++)
    {
        if( lastInputDevice == m_AudioInputDevices[i].Name )
        {
            Log( "Last-In-Index: %i", m_AudioInputDevices[i].NumericID );
            m_InputDeviceIndex = i;
            break;
        }
    }

    // and if it's in the current device-list, then choose it again
    for( unsigned int i=0; i<m_AudioOutputDevices.size(); i++)
    {
        if( lastOutputDevice == m_AudioOutputDevices[i].Name )
        {
            Log( "Last-Out-Index: %i", m_AudioOutputDevices[i].NumericID );
            m_OutputDeviceIndex = i;
            break;
        }
    }
}

GlobalConfig::~GlobalConfig()
{
}

void GlobalConfig::chooseSoundDevices()
{
    bool lastState = m_AudioIsConfigured;

    wxCustomConfigDialog configDialog;
    configDialog.ShowModal();

    // flag audio to be configured, now...
    m_AudioIsConfigured = true;

    // if it also was before and the mixer-thread is already up and running,
    // then reconfigure it...
    if( m_AudioThread and m_AudioThread->IsRunning() and lastState )
    {
        m_AudioThread->Reconfigure();
    }
}

bool GlobalConfig::startAudioThread()
{
    if( not m_AudioIsConfigured )
    {
        // if audio-IO is not configured by now, then do it...
        chooseSoundDevices();
    }

    // ------------------------------------------------------------------------
    // Create Audio-Thread
    // ------------------------------------------------------------------------
    m_AudioThread = new AudioThread();
    if( m_AudioThread->Create() != wxTHREAD_NO_ERROR )
    {
        Log( "Can't create audio-thread." );
        return( false );
    }

    // Try to set it to have a high priority (if we *can* set it, which is not
    // always the case )
    //m_AudioThread->SetPriority(100);

    // ... and really start it ...
    if( m_AudioThread->Run() != wxTHREAD_NO_ERROR)
    {
        Log( "Can't start audio-thread." );
        return( false );
    }

    // return true on success...
    return( true );
}

bool GlobalConfig::stopAudioThread()
{
    if( m_AudioThread && m_AudioThread->IsRunning() )
    {
        m_AudioThread->terminate();
        while( m_AudioThread->IsRunning() )
        {
            wxMilliSleep( 100 );
        }
    }

    m_LoggingWindow->Close();
    m_LoggingWindow->Destroy();

    return( false );
}

std::vector <AudioDevice_t> GlobalConfig::getOutputDevices()
{
    return( m_AudioOutputDevices );
}

std::vector <AudioDevice_t> GlobalConfig::getInputDevices()
{
    return( m_AudioInputDevices );
}

int GlobalConfig::getOutputDevice()
{
    return( m_OutputDeviceIndex );
}

int GlobalConfig::getInputDevice()
{
    return( m_InputDeviceIndex );
}

int GlobalConfig::getOutputDevice_PA()
{
    return( m_AudioOutputDevices[m_OutputDeviceIndex].NumericID );
}

int GlobalConfig::getInputDevice_PA()
{
    return( m_AudioInputDevices[m_InputDeviceIndex].NumericID );
}

double GlobalConfig::getInputSampleRate()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning() )
    {
        return( m_AudioThread->getSampleRate() );
    }

    return( 0 );
}

double GlobalConfig::getProcessingSampleRate()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        return( m_AudioThread->getSampleRate() );
    }

    return( 0 );
}

void GlobalConfig::setOutputDevice(int index)
{
    Log( "GlobalConfig::setOutputDevice(%i);", index );
    m_Registry.setOutputDevice( m_AudioOutputDevices[index].Name );
    m_OutputDeviceIndex=index;
}

void GlobalConfig::setInputDevice(int index)
{
    Log( "GlobalConfig::setInputDevice(%i);", index );
    m_Registry.setInputDevice( m_AudioInputDevices[index].Name );
    m_InputDeviceIndex=index;
}

AudioQueue* GlobalConfig::getFFTQueueRF()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        return( m_AudioThread->getFFTQueueRF() );
    }

    return( 0 );
}

AudioQueue* GlobalConfig::getFFTQueueAF()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        return( m_AudioThread->getFFTQueueAF() );
    }

    return( 0 );
}

void GlobalConfig::setFilter( float bandwidth )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setFilter( bandwidth );
    }
}

void GlobalConfig::setTune( float frequency )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setTune( frequency );
    }
}

void GlobalConfig::setMode( SDRAudio::SDR_MODE mode )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setMode( mode );
    }
}

void GlobalConfig::setANF( double value )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setANF( value );
    }
}

void GlobalConfig::setDNR( double value )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setDNR( value );
    }
}

float GlobalConfig::getSignalLevel()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        return( m_AudioThread->getSignalLevel() );
    }
    return( 0.f );
}

float GlobalConfig::getSquelchLevel()
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        return( m_AudioThread->getSquelchLevel() );
    }
    return( 0.f );
}

void GlobalConfig::setSquelchLevel(float level)
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setSquelchLevel(level);
    }
}

void GlobalConfig::setAGCTime( double upTime, double downTime )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        m_AudioThread->setAGCTime( upTime, downTime  );
    }
}

void GlobalConfig::setATTdB( double AttenuatorValue )
{
    if( m_AudioThread and
		m_AudioThread->IsRunning())
    {
        float factor = 1.0/powf(10.f,AttenuatorValue/20.f);
        m_AudioThread->setATTCompensation(factor);
    }
}
