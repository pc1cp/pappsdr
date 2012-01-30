#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <vector>

#include "wx/wx.h"
#include "audiothread.hpp"

// Audio-Device-Struct...
typedef struct
{
    wxString Name;
    int NumericID;
} AudioDevice_t;

class GlobalConfig
{
    // make singleton (C & CC are private)
    private:
    GlobalConfig();
    GlobalConfig( const GlobalConfig& ) {};

    public:
    ~GlobalConfig();

    static GlobalConfig* getInstance()
    {
        static GlobalConfig Instance;
        return( &Instance );
    }

    bool startAudioThread();
    void chooseSoundDevices();
    std::vector<AudioDevice_t> getOutputDevices();
    std::vector<AudioDevice_t> getInputDevices();
    int getInputDevice();
    int getOutputDevice();
    int getInputDevice_PA();
    int getOutputDevice_PA();
    double getInputSampleRate();
    double getProcessingSampleRate();

    void setMode( SDRAudio::SDR_MODE mode );

    void setOutputDevice(int index);
    void setInputDevice(int index);

    void setTune( float freq );

    void setAGCTime( double upTime, double downTime );
    void setATTdB  ( double AttenuatorValue );

    void  setSLevelCorrection( float dBValue ){ m_SLevelCorrection=dBValue; }
    float getSLevelCorrection(){ return m_SLevelCorrection; }

    AudioQueue* getFFTQueueRF();
    AudioQueue* getFFTQueueAF();

    float       getSignalLevel();
    float       getSquelchLevel();
    void        setSquelchLevel(float level);
    void        setFilter( float bandwidth );

    private:

    // S-Meter-Correction

    // List of Output-Devices
    std::vector <AudioDevice_t> m_AudioOutputDevices;
    int                         m_OutputDeviceIndex;

    // List of Input-Devices
    std::vector <AudioDevice_t> m_AudioInputDevices;
    int                         m_InputDeviceIndex;

    bool                        m_AudioIsConfigured;

    AudioThread*  m_AudioThread;

    float                       m_SLevelCorrection;
};

#endif
