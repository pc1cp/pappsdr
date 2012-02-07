
#include "config.hpp"
#include "sounddevicedialog.hpp"

#include <ciso646>

GlobalConfig::GlobalConfig()
{
    m_AudioThread = 0;

    // initializing portaudio
    PaError err = Pa_Initialize();

    if( err != paNoError )
    {
        std::cerr << "Error initializing Portaudio: " << Pa_GetErrorText( err )
                  << "\n";
        exit(-1);
    }

    // get available soundcards from portaudio
    int nDevices;

    nDevices = Pa_GetDeviceCount();
    if( nDevices < 0 )
    {
        std::cerr << "Error, Pa_CountDevices returned " << nDevices << "\n";
        err = nDevices;
        std::cerr << "Error initializing Portaudio: " << Pa_GetErrorText( err )
                  << "\n";
        exit(-1);
    }
    else
    {
        // Ok, so we got some audio-devices, it's time to test if they support
        // what we need...

        const PaDeviceInfo* DeviceInfo;

        // Build up list of usable output devices...
        for( int i=0; i<nDevices; i++ )
        {
            DeviceInfo = Pa_GetDeviceInfo( i );

            if( DeviceInfo->maxOutputChannels >= 2 )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioOutputDevices.push_back( entry );
            }
        }

        // Build up list of usable input devices...
        for( int i=0; i<nDevices; i++ )
        {
            DeviceInfo = Pa_GetDeviceInfo( i );

            if( DeviceInfo->maxInputChannels >= 2 )
            {
                // add audio-device to list of usable audio-devices.
                AudioDevice_t entry;
                entry.Name = wxString::From8BitData(DeviceInfo->name);
                entry.NumericID = i;
                m_AudioInputDevices.push_back( entry );
            }
        }
    }

    std::cerr << "\n\nFound this list of usable *INPUT* Audio-Devices:\n\n";
    for( unsigned int i=0; i<m_AudioInputDevices.size(); i++)
    {
        std::cerr << "Index: "        << m_AudioInputDevices[i].NumericID
                  << "\tDevicename: " << m_AudioInputDevices[i].Name.mb_str()
                  << "\n";
    }

    std::cerr << "\n\nFound this list of usable *OUTPUT* Audio-Devices:\n\n";
    for( unsigned int i=0; i<m_AudioOutputDevices.size(); i++)
    {
        std::cerr << "Index: "        << m_AudioOutputDevices[i].NumericID
                  << "\tDevicename: " << m_AudioOutputDevices[i].Name.mb_str()
                  << "\n";
    }

    m_OutputDeviceIndex =  0;
    m_InputDeviceIndex  =  0;
}

GlobalConfig::~GlobalConfig()
{
}

void GlobalConfig::chooseSoundDevices()
{
    bool lastState = m_AudioIsConfigured;

    MySoundDeviceDialog SoundDeviceDialog(0);

    if ( SoundDeviceDialog.ShowModal() == wxID_OK )
    {
        std::cerr << "OK pressed\n";
    }

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
        wxLogStatus( _("Can't create audio-thread.") );
        return( false );
    }

    // Try to set it to have a high priority (if we *can* set it, which is not
    // always the case )
    //m_AudioThread->SetPriority(100);

    // ... and really start it ...
    if( m_AudioThread->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogStatus( _("Can't start audio-thread." ) );
        return( false );
    }

    // return true on success...
    return( true );
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
    if( m_AudioThread )
    {
        return( m_AudioThread->getInputSoundCardSampleRate() );
    }

    return( 0 );
}

double GlobalConfig::getProcessingSampleRate()
{
    if( m_AudioThread )
    {
        return( m_AudioThread->getProcessingSampleRate() );
    }

    return( 0 );
}

void GlobalConfig::setOutputDevice(int index)
{
    std::cerr << "Output-Device-Index: " << index << "\n";
    m_OutputDeviceIndex=index;
}

void GlobalConfig::setInputDevice(int index)
{
    std::cerr << "Input-Device-Index: " << index << "\n";
    m_InputDeviceIndex=index;
}

AudioQueue* GlobalConfig::getFFTQueueRF()
{
    if( m_AudioThread )
    {
        return( m_AudioThread->getFFTQueueRF() );
    }

    return( 0 );
}

AudioQueue* GlobalConfig::getFFTQueueAF()
{
    if( m_AudioThread )
    {
        return( m_AudioThread->getFFTQueueAF() );
    }

    return( 0 );
}

void GlobalConfig::setFilter( float bandwidth )
{
    if( m_AudioThread )
    {
        m_AudioThread->setFilter( bandwidth );
    }
}

void GlobalConfig::setTune( float frequency )
{
    if( m_AudioThread )
    {
        m_AudioThread->setTune( frequency );
    }
}

void GlobalConfig::setMode( SDRAudio::SDR_MODE mode )
{
    if( m_AudioThread )
    {
        m_AudioThread->setMode( mode );
    }
}

float GlobalConfig::getSignalLevel()
{
    if( m_AudioThread )
    {
        return( m_AudioThread->getSignalLevel() );
    }
    return( 0.f );
}

float GlobalConfig::getSquelchLevel()
{
    if( m_AudioThread )
    {
        return( m_AudioThread->getSquelchLevel() );
    }
    return( 0.f );
}

void GlobalConfig::setSquelchLevel(float level)
{
    if( m_AudioThread )
    {
        m_AudioThread->setSquelchLevel(level);
    }
}

void GlobalConfig::setAGCTime( double upTime, double downTime )
{
    if( m_AudioThread )
    {
        m_AudioThread->setAGCTime( upTime, downTime  );
    }
}

void GlobalConfig::setATTdB( double AttenuatorValue )
{
    if( m_AudioThread )
    {
        float factor = 1.0/powf(10.f,AttenuatorValue/20.f);
        m_AudioThread->setATTCompensation(factor);
    }
}
