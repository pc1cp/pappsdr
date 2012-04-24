#include "audiothread.hpp"
#include "config.hpp"

#include <iostream>
#include <ciso646>


double  AudioThread::s_TestSampleRates[]    = { 192000.0,
                                                176400.0,
                                                 96000.0,
                                                 88200.0,
                                                 48000.0,
                                                 44100.0 };
int     AudioThread::s_NrOfTestSampleRates  = 1;

AudioThread::AudioThread()
{
   	wxMutexLocker lock( m_ThreadMutex );
	wxLogStatus( _("AudioThread::AudioThread();") );

    m_SDRAudio = 0;

    m_ReconfigureFlag = false;

    m_Latency = 500.0/1000.0; // min 10ms!!

    // set AudioQueues to 1 seconds buffer on maximum allowed Rate
    m_OutputQueue.resize( 1000000 );
    m_InputQueue.resize ( 1000000 );
    m_AudioFFTQueue.resize ( 1000000 );

	wxLogStatus( _("    AudioQueues set up.") );

    // set initial AudioState to UNCONFIGURED/CLOSED
    m_AudioState = STATE_CLOSED;

    // set Attenuator = 0.0 dB
    m_AttenuatorValue = 1.f;

	wxLogStatus( _("        OutputQueue-Size = %i"),   m_OutputQueue.size() );
	wxLogStatus( _("         InputQueue-Size = %i"),    m_InputQueue.size() );
	wxLogStatus( _("       AF-FFT-Queue-Size = %i"), m_AudioFFTQueue.size() );
}

void* AudioThread::Entry()
{
	wxLogStatus( _("AudioThread::Entry();") );
    bool done = false;

    m_PaStreamIsActive = false;

    int cnt=0;

    while( not done )
    {
        // Check Audio-Device-State every 100 ms
        switch( m_AudioState )
        {
            case STATE_CLOSED:
            {
                wxLogStatus( _("Audiothread-STATE: CLOSED") );
                // closed ?!? Then try to configure it...
                Configure();
                break;
            }
            case STATE_OPEN:
            {
                wxLogStatus( _("Audiothread-STATE: OPEN") );

                // Open, but not running? Then start it by all means...
                Pa_StartStream( m_PaStream );
                m_PaStreamIsActive = true;

                // change audio-state to running...
                m_AudioState = STATE_RUNNING;
                break;
            }
            case STATE_ERROR:
            {
				// close all active streams ...
                if( m_PaStreamIsActive )
                {
                    Pa_CloseStream( m_PaStream );
                    m_PaStreamIsActive = false;
                }

                // send an event, so the user can notice that there is something
                // going wrong badly...
                //wxWindow* statusWindow = Config->getStatusWindow();
                //wxCommandEvent event(wxEVT_COMMAND_MIXER_ERROR);
                //statusWindow->AddPendingEvent( event );

                // now wait here(!) until the user changes the configuration
                // again
                while( m_AudioState == STATE_ERROR )
                {
                    wxLogStatus( _("Audiothread-STATE: *** ERROR ***") );

                    // Check for reconfiguration-request...
                    m_ReconfigureLock.Lock();
                    if( m_ReconfigureFlag )
                    {
                        // reset flag
                        m_ReconfigureFlag = false;

                        // change AudioState
                        m_AudioState = STATE_CLOSED;
                    }
                    m_ReconfigureLock.Unlock();
                    Sleep( 1000 ); // wait a second
                }
                break;
            }
            case STATE_RUNNING:
            default:
            {
                // do nothing but wait for reconfigure-flag... and check that
                // all audio-streams are up and running...

                cnt = (cnt+1)%40;
                if( cnt==0 )
                wxLogStatus( _("Audiothread-STATE: RUNNING") );

                // Check for reconfiguration-request...
                m_ReconfigureLock.Lock();
                if( m_ReconfigureFlag )
                {
                    // close all active streams and change state...
                    if( m_PaStreamIsActive )
                    {
                        Pa_CloseStream( m_PaStream );
                        m_PaStreamIsActive = false;
                    }

                    // reset flag
                    m_ReconfigureFlag = false;

                    // change AudioState
                    m_AudioState = STATE_CLOSED;
                }
                m_ReconfigureLock.Unlock();

                // Check that everything is OK with the streams...

                if( m_PaStreamIsActive )
                {
                    if( Pa_IsStreamActive( m_PaStream ) != 1 )
                    {
                        m_AudioState = STATE_ERROR;
                    }
                }
            }
        }
        Sleep( 2500 ); // wait for 2500 ms
    }

    // all finished here...
    return( 0 );
}

void    AudioThread::Reconfigure()
{
    // just flag, that we need to reconfigure the audio-hardware
    m_ReconfigureLock.Lock();
    m_ReconfigureFlag = true;
    m_ReconfigureLock.Unlock();
}

void    AudioThread::Configure()
{
	wxLogStatus( _("AudioThread::Configure();") );
    GlobalConfig* config = GlobalConfig::getInstance();

    // Try to configure Audio-Hardware
    bool everythingIsOk = true;

    bool outputIsOpen   = false;

    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // test desired output-device for allowed sample-rates
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    int PortaudioDeviceIndexOut = config->getOutputDevice_PA();
    int PortaudioDeviceIndexIn  = config->getInputDevice_PA ();

    m_SampleRate = config->getSampleRate();

	wxLogStatus( _("Input-Device = %i"), PortaudioDeviceIndexIn );
	wxLogStatus( _("Output-Device = %i"), PortaudioDeviceIndexOut );
	wxLogStatus( _("Sample-Rate = %f"), m_SampleRate );

    // if we got a valid result, then open the output-device
    if( m_SampleRate > 0 and everythingIsOk )
    {
        // all parameters in m_PaParameters are set, now, so we just need to
        // setup the callback-address and decide how many frames per buffer
        // we want to have...

        memset( &m_PaParametersIn,  0, sizeof( m_PaParametersIn ) );
        memset( &m_PaParametersOut, 0, sizeof( m_PaParametersOut ) );

        double inputLatency  = Pa_GetDeviceInfo( PortaudioDeviceIndexOut )
                               ->defaultLowInputLatency;
        double outputLatency = inputLatency;

        std::cerr << "default-Latency-In : " <<  inputLatency*m_SampleRate << "\n";
        std::cerr << "default-Latency-Out: " << outputLatency*m_SampleRate << "\n";

        m_FramesPerBuffer = inputLatency*m_SampleRate/10.0;

        m_PaParametersOut.device                    = PortaudioDeviceIndexOut;
        m_PaParametersOut.channelCount              = 2;
        m_PaParametersOut.suggestedLatency          = outputLatency*3;
        m_PaParametersOut.sampleFormat              = paFloat32;
        m_PaParametersOut.hostApiSpecificStreamInfo = NULL;

        m_PaParametersIn.device                     = PortaudioDeviceIndexIn;
        m_PaParametersIn.channelCount               = 2;
        m_PaParametersIn.suggestedLatency           = inputLatency*3;
        m_PaParametersIn.sampleFormat               = paFloat32;
        m_PaParametersIn.hostApiSpecificStreamInfo  = NULL;

        int err = Pa_OpenStream( &m_PaStream,
                                 &m_PaParametersIn,
                                 &m_PaParametersOut,
                                 m_SampleRate,
                                 m_FramesPerBuffer,
                                 paNoFlag,
                                 &(this->PaCallback),
                                 (void *)this );

        if( err != paNoError )
        {
            std::cerr << "\n*** \n";
            std::cerr << "*** Portaudio-Error when opening Device:\n";
            std::cerr << "*** \n";
            std::cerr << "*** " << Pa_GetErrorText(err) << "\n";
            std::cerr << "*** \n\n";
			wxLogStatus( _("Error opening AudioDevice") );
            everythingIsOk = false;
        }
        else
        {
            outputIsOpen = true;
        }
    }
    else
    {
        std::cerr << "\n*** \n";
        std::cerr << "*** Samplerate-Test failed on OutputDevice.\n";
        std::cerr << "*** \n";
        everythingIsOk = false;
    }

    if( everythingIsOk )
    {
        // Yessss.... we're open, now... ;-)
      	wxMutexLocker lock( m_ThreadMutex );

        m_AudioState = STATE_OPEN;
        std::cerr << "Sample-Rate  = " << m_SampleRate << "\n";

        delete m_SDRAudio;
        m_SDRAudio = new SDRAudio( m_SampleRate );

    }
    else
    {
        // oops... something went bad... so, we close all already opened audio-
        // devices (if any)...
        if( outputIsOpen )
        {
            Pa_CloseStream( &m_PaStream );
        }

        m_AudioState = STATE_ERROR;
    }
}

double  AudioThread::GetSampleRate( int PortaudioDeviceIndex )
{
    // test for allowed samplerates...
    for( int n=0; n<s_NrOfTestSampleRates; n++ )
    {
        double const TestRate = s_TestSampleRates[n];

        // clear Output-Parameter-block ...
        memset( &m_PaParametersIn, 0, sizeof( m_PaParametersIn ) );

        // set test-parameters
        m_PaParametersIn.device              = PortaudioDeviceIndex;
        m_PaParametersIn.channelCount        = 2;
        m_PaParametersIn.suggestedLatency    = m_Latency;
        m_PaParametersIn.sampleFormat        = paFloat32;

        // test for given samplerate
        int err = Pa_IsFormatSupported( 0, &m_PaParametersIn, TestRate );

        // if supported, return first match
        if( err == paFormatIsSupported )
        {
            return( TestRate );
        }
    }
    // uhh,... this should not happen...
    return( -1.0 );
}

void AudioThread::setTune( float frequency )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setTune( frequency );
    }
}

AudioQueue* AudioThread::getFFTQueueRF()
{
	wxMutexLocker lock( m_ThreadMutex );
    return( &m_InputQueue );
}

AudioQueue* AudioThread::getFFTQueueAF()
{
	wxMutexLocker lock( m_ThreadMutex );
    return( &m_AudioFFTQueue );
}

double AudioThread::getSampleRate()
{
	wxMutexLocker lock( m_ThreadMutex );
    return( m_SampleRate );
}

#include <cstdlib>

int decimator = 0;

int AudioThread::PaCallback( const void * iDataPtr,
                                     // input-samples (not used for output)
                                     void *        oDataPtr,
                                     // output-samples
                                     unsigned long frames,
                                     // nr of samples we need to deliver
                                     const PaStreamCallbackTimeInfo *,
                                     // timeInfo,
                                     PaStreamCallbackFlags,
                                     // statusFlags,
                                     void* instance
                                     // caller-instance (mixer-thread)
                                   )
{
    // this makes things a lot easier...
    AudioThread*    _this_ = (AudioThread*)instance;
    ComplexSample*  iData  = (ComplexSample*)iDataPtr;
    ComplexSample*  oData  = (ComplexSample*)oDataPtr;

    // update Waterfall/FFT-Buffer
    _this_->m_InputQueue.lock();
    for( unsigned long n=0; n<frames; ++n )
    {
        ComplexSample iSample = iData[n]*_this_->m_AttenuatorValue;
        _this_->m_InputQueue.put( iSample );
    }
    _this_->m_InputQueue.unlock();

    // Process audio
    for( unsigned long n=0; n<frames; ++n )
    {
        ComplexSample iSample = iData[n]*_this_->m_AttenuatorValue;
        oData[n] = _this_->m_SDRAudio->demodulate( iSample );
    }

    // update Waterfall/FFT-Buffer for decoded audio
    double coreRate = _this_->m_SDRAudio->getSampleRate();
    double inputRate = _this_->m_SDRAudio->getSampleRate();

    int ratio = inputRate/6000.0;// inputRate/coreRate;

    _this_->m_AudioFFTQueue.lock();
    for( unsigned long n=0; n<frames; ++n )
    {
        if( decimator == 0 )
        {
            _this_->m_AudioFFTQueue.put( oData[n] );
        }
        decimator = (decimator+1)%ratio;
    }
    _this_->m_AudioFFTQueue.unlock();

#if 0
    bool hasData = true;
    _this_->m_OutputQueue.lock();
    if( _this_->m_OutputQueue.getLength() < frames )
    {
        hasData = false;
    }
    _this_->m_OutputQueue.unlock();

    if( !hasData ) return( 0 );

    static ComplexSample buffer[4];
    static double sample = 0;

    double kp = 0.00000005000;
    double ki = 0.00000000125;
    double kd = 0.00000002500;

    static double error_last = 0;
    static double error = 0;
    static double error_sum = 0;
    static double rateController = 1.0;

    double ratio = _this_->m_SDRAudio->getOutputSampleRate() /
                   _this_->m_SampleRate;
           ratio *= rateController;

//std::cerr << "\nratio: " << ratio << "\n";
    // put the samples into the input-queue
    for( int n=0; n<frames; ++n )
    {
        //_this_->m_OutputQueue.pull();
        oData[n] = buffer[0]*(1.0-sample)+
                   buffer[1]*(    sample);
        sample += ratio;
        while( sample >= 1.0 )
        {
            sample -= 1.0;
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
            buffer[2] = buffer[3];
            _this_->m_OutputQueue.lock();
            buffer[3] = _this_->m_OutputQueue.pull();
            _this_->m_OutputQueue.unlock();
        }
    }

    error_last = error;
    _this_->m_OutputQueue.lock();
    error = _this_->m_OutputQueue.getTimingError()*-1;
    _this_->m_OutputQueue.unlock();
    error_sum += error;

    rateController = 1.0 + kp*error + ki*error_sum +  kd*( error - error_last );
    rateController = rateController<0.5? 0.5:rateController;
    rateController = rateController>20.0? 20.0:rateController;
#endif
    return( 0 );
}

void AudioThread::setFilter( float bandwidth )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setFilter( bandwidth );
    }
}

void AudioThread::setMode( SDRAudio::SDR_MODE mode )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setMode( mode );
    }
}

float AudioThread::getSignalLevel()
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        return( m_SDRAudio->getSignalLevel() );
    }

    return( 0.f );
}

float AudioThread::getSquelchLevel()
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        return( m_SDRAudio->getSquelchLevel() );
    }

    return( 0.f );
}

void AudioThread::setSquelchLevel( float level )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setSquelchLevel( level );
    }
}

void AudioThread::setAGCTime( double upTime, double downTime )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setAGCTime( upTime, downTime  );
        wxLogStatus( _("AGC-Uptime = %f"), upTime );
    }
}

void AudioThread::setANF( double value )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setANF( value );
    }
}

void AudioThread::setDNR( double value )
{
	wxMutexLocker lock( m_ThreadMutex );
    if( m_SDRAudio )
    {
        m_SDRAudio->setDNR( value );
    }
}
