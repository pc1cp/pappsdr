#ifndef __AUDIOTHREAD_HPP__
#define __AUDIOTHREAD_HPP__

#ifndef _WIN32
	#include "portaudio.h"
#else
	#define PA_USE_WDMKS 1
	#include "pa_win_wdmks.h"
#endif

//#include "sample.hpp"
#include "complexsample.hpp"
#include "audioqueue.hpp"
#include "wx/thread.h"
#include "wx/event.h"

#include "libsdraudio.hpp"

extern const wxEventType wxEVT_COMMAND_AUDIOTHREAD_ERROR;

class AudioThread: public wxThread
{
    public:

    AudioThread                 ();
    virtual void* Entry         ();
    void    Reconfigure         ();
    void    terminate           ();

    void    setMode             ( SDRAudio::SDR_MODE );
    void    setFilter           ( float bandwidth );

    void    setTune             ( float frequency );

    void    setATTCompensation  ( float attenuatorValue )
                                { m_AttenuatorValue = attenuatorValue; }

    void    setAGCTime          ( double upTime, double downTime );

    AudioQueue* getFFTQueueRF   ();
    AudioQueue* getFFTQueueAF   ();

    float       getSignalLevel  ();
    float       getInputLevel   ();
    float       getOutputLevel  ();
    float       getSquelchLevel ();
    void        setSquelchLevel (float level);

    void        setANF( double value );
    void        setDNR( double value );

    double  getSampleRate();

    private:

    bool    m_Done;

    enum AUDIOSTATE
    {
        STATE_ERROR,
        STATE_CLOSED,
        STATE_OPEN,
        STATE_RUNNING
    };

    enum AUDIOSTATE     m_AudioState;

    void    Configure           ();
    double  GetSampleRate       (int PortaudioDeviceIndex );

    SDRAudio*           m_SDRAudio;
	//wxMutex				m_SDRAudioMutex;

    double              m_Latency;
    int                 m_FramesPerBuffer;

    double              m_SampleRate;

    PaStreamParameters  m_PaParametersIn;
    PaStreamParameters  m_PaParametersOut;
    PaStream*           m_PaStream;

    bool                m_PaStreamIsActive;

    static double       s_TestSampleRates[];
    static int          s_NrOfTestSampleRates;

    static AudioQueue          m_OutputQueue;
    static AudioQueue          m_InputQueue;
    static AudioQueue          m_AudioFFTQueue;

    float               m_AttenuatorValue;

    float               m_InputLevel;
    float               m_OutputLevel;

    wxMutex             m_ReconfigureLock;
    bool                m_ReconfigureFlag;

    wxMutex             m_ThreadMutex;

    static int PaCallback( const void *input,
                                 void *output,
                        unsigned long frameCount,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                                void *userData );

};

#endif
