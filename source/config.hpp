#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <vector>

#include "wx/wx.h"
#include "wx/thread.h"
#include "wx/config.h"
#include "wx/fileconf.h"
#include "wx/datetime.h"
//#include "wx/msw/regconf.h"
#include "audiothread.hpp"

// Audio-Device-Struct...
typedef struct
{
    wxString Name;
    int NumericID;
} AudioDevice_t;

// structure to hold frequency-dependend Pappradio-Settings
typedef struct
{
    double          m_Fmin;
    double          m_Fmax;
    int             m_AttenuatorIndex;
    int             m_FilterIndex;
} FrequencySettings_t;

class ConfigRegistry
{
    public:
    ConfigRegistry()
    {
        // load settings ------------------------------------------------------
        loadSettings();
    }

    ~ConfigRegistry()
    {
        // save settings ------------------------------------------------------
        saveSettings();
    }

    void   setSampleRate( double value ){ m_AudioSampleRate = value; saveSettings();   }
    double getSampleRate( )             { return( m_AudioSampleRate );  }

    void   setSampleRatePPM( double value ){ m_AudioSampleRateErrorPPM = value; saveSettings(); }
    double getSampleRatePPM( )             { return( m_AudioSampleRateErrorPPM ); }

    void   setInputDevice( wxString deviceName ) { m_AudioInputDevice.Name = deviceName; saveSettings(); }
    wxString getInputDevice( ) { return m_AudioInputDevice.Name; }

    void   setOutputDevice( wxString deviceName ) { m_AudioOutputDevice.Name = deviceName; saveSettings(); }
    wxString getOutputDevice( ) { return m_AudioOutputDevice.Name; }

    void    setXTalPPM( double value ){ m_PappradioXTALPPM = value; saveSettings(); }
    double  getXTalPPM() { return m_PappradioXTALPPM; }

    void   setAttValue( int index, double valueDB ){ m_AttenuatorRealDB[index&3] = valueDB; saveSettings();};
    double getAttValue( int index ){ return ( m_AttenuatorRealDB[index&3] ); }

    void    setCurrentAttenuator( int index ){ m_AttenuatorIndex = (int)index; saveSettings(); }
    int     getCurrentAttenuator(){ return ( (int)m_AttenuatorIndex ); }

    void    setCurrentMode( int index ){ m_ModeIndex = (int)index; saveSettings();}
    int     getCurrentMode(){ return ( (int)m_ModeIndex ); }

    void    setCurrentLOFrequency( double frequency ){ m_LOFrequency = frequency; saveSettings();}
    int     getCurrentLOFrequency(){ return ( (int)m_LOFrequency ); }

    void    setCurrentVFOFrequency( double frequency ){ m_VFOFrequency = frequency; saveSettings(); }
    int     getCurrentVFOFrequency(){ return ( (int)m_VFOFrequency ); }

    private:

    void        loadSettings()
    {
        m_Config = new wxConfig(_("PappSDR") );

        m_Config->Read( _("SampleRate"),
                        _("44100")       ).ToDouble
                        ( &m_AudioSampleRate );

        m_Config->Read( _("SampleRateError"),
                        _("0.0")           ).ToDouble
                        ( &m_AudioSampleRateErrorPPM );

        m_Config->Read( _("PappradioXTALError"),
                        _("0.0")           ).ToDouble
                        ( &m_PappradioXTALPPM );

        m_Config->Read( _("PappradioAtt00dB"),
                        _("0.0")           ).ToDouble
                        ( &m_AttenuatorRealDB[0] );

        m_Config->Read( _("PappradioAtt10dB"),
                        _("-12.0")           ).ToDouble
                        ( &m_AttenuatorRealDB[1] );

        m_Config->Read( _("PappradioAtt20dB"),
                        _("-24.0")           ).ToDouble
                        ( &m_AttenuatorRealDB[2] );

        m_Config->Read( _("PappradioAtt30dB"),
                        _("-36.0")           ).ToDouble
                        ( &m_AttenuatorRealDB[3] );

        m_Config->Read( _("CurrentAttenuatorIndex"),
                        _("3")                     ).ToLong
                        ( &m_AttenuatorIndex );

        m_Config->Read( _("CurrentModeIndex"),
                        _("1")               ).ToLong
                        ( &m_ModeIndex );

        m_Config->Read( _("CurrentLOFrequency"),
                        _("7100000")           ).ToDouble
                        ( &m_LOFrequency );

        m_Config->Read( _("CurrentVFOFrequency"),
                        _("10000")              ).ToDouble
                        ( &m_VFOFrequency );

        m_AudioInputDevice.Name  = m_Config->Read( _("AudioInputDeviceName"),
                                                   _("") );

        m_AudioOutputDevice.Name = m_Config->Read( _("AudioOutputDeviceName"),
                                                   _("") );

        delete m_Config;
    }

    void        saveSettings()
    {
        wxString valueString;
        m_Config = new wxConfig(_("PappSDR") );

        valueString.Printf( _("%f"), m_AudioSampleRate );
        m_Config->Write( _("SampleRate"), valueString );

        valueString.Printf( _("%f"), m_AudioSampleRateErrorPPM );
        m_Config->Write( _("SampleRateError"), valueString );

        valueString.Printf( _("%f"), m_PappradioXTALPPM );
        m_Config->Write( _("PappradioXTALError"), valueString );

        valueString.Printf( _("%f"), m_AttenuatorRealDB[0] );
        m_Config->Write( _("PappradioAtt00dB"), valueString );

        valueString.Printf( _("%f"), m_AttenuatorRealDB[1] );
        m_Config->Write( _("PappradioAtt10dB"), valueString );

        valueString.Printf( _("%f"), m_AttenuatorRealDB[2] );
        m_Config->Write( _("PappradioAtt20dB"), valueString );

        valueString.Printf( _("%f"), m_AttenuatorRealDB[3] );
        m_Config->Write( _("PappradioAtt30dB"), valueString );

        valueString.Printf( _("%i"), m_AttenuatorIndex );
        m_Config->Write( _("CurrentAttenuatorIndex"), valueString );

        valueString.Printf( _("%i"), m_ModeIndex );
        m_Config->Write( _("CurrentModeIndex"), valueString );

        valueString.Printf( _("%f"), m_LOFrequency );
        m_Config->Write( _("CurrentLOFrequency"), valueString );

        valueString.Printf( _("%f"), m_VFOFrequency );
        m_Config->Write( _("CurrentVFOFrequency"), valueString );

        m_Config->Write( _("AudioInputDeviceName"),
                         m_AudioInputDevice.Name );

        m_Config->Write( _("AudioOutputDeviceName"),
                         m_AudioOutputDevice.Name );

        m_Config->Flush();
        delete m_Config;
    }

    // Audio-Devices ----------------------------------------------------------
    AudioDevice_t                       m_AudioInputDevice;
    AudioDevice_t                       m_AudioOutputDevice;

    // Audio-Sample-Rate and Sample-Rate-Error in PPM -------------------------
    double                              m_AudioSampleRate;
    double                              m_AudioSampleRateErrorPPM;

    // Last used state --------------------------------------------------------
    double                              m_LOFrequency;
    double                              m_VFOFrequency;
    long                                m_AttenuatorIndex;
    long                                m_ModeIndex;

    // Tuning-Error due to XTAL-Frequency-Deviation in PPM --------------------
    double                              m_PappradioXTALPPM;

    // Frequency-Settings-Table -----------------------------------------------
    std::vector<FrequencySettings_t>    m_FrequencySettings;

    // Attenuator-Correction-Values -------------------------------------------
    double                              m_AttenuatorRealDB[4];

    // some place to store all this -------------------------------------------
    wxConfig*                           m_Config;
};

class GlobalLogging : public wxFrame
{
    public:
    GlobalLogging()
    : wxFrame(NULL, wxID_ANY, _("PappSDR-Logging") )
    {
        wxBoxSizer* topSizer = new wxBoxSizer( wxVERTICAL );
        m_TextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(640,480), wxTE_MULTILINE );
        topSizer->Add( m_TextCtrl );
        SetSizerAndFit( topSizer );
        Show();
        m_LogFile.open( "PappSDR_LogFile.txt", std::ios::out );
        m_Timer = new wxTimer( this, 0 );
        m_Timer->Start( 100 );
    }

    virtual ~GlobalLogging()
    {
        m_LogFile.close();
    }

    void logMessage( std::string text )
    {
        wxDateTime thisTime = wxDateTime::GetTimeNow();
        wxDateTime::wxDateTime_t hour   = thisTime.GetHour  ();
        wxDateTime::wxDateTime_t minute = thisTime.GetMinute();
        wxDateTime::wxDateTime_t second = thisTime.GetSecond();

        wxString timeStamp;
        timeStamp.Printf( _("[ %02i:%02i:%02i ]   "), hour, minute, second );
        wxMutexLocker lock( m_LogMutex );
        m_LogFile << timeStamp.c_str() << text;
        m_MessageStack.push_back( std::string(timeStamp.mb_str())+text );
    }

    void onTimer( wxTimerEvent& WXUNUSED(event) )
    {
        wxMutexLocker lock( m_LogMutex );
        for( unsigned int n=0; n<m_MessageStack.size(); ++n )
        {
            wxString message = wxString( m_MessageStack[n].c_str(), wxConvUTF8 );
            *m_TextCtrl << message << _("\n");
        }
        m_MessageStack.clear();
    }

    void onClose( wxCloseEvent& WXUNUSED(event) )
    {
        // do not(!) close this window. Just hide it.
        Hide();
    }

    private:
    wxMutex         m_LogMutex;
    wxTextCtrl*     m_TextCtrl;

    std::vector<std::string> m_MessageStack;

    wxTimer*        m_Timer;

    std::fstream    m_LogFile;

    DECLARE_EVENT_TABLE();
};

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
    bool stopAudioThread();

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

    void   setAttValue( int index, double valueDB ){ m_Registry.setAttValue( index, valueDB ); };
    double getAttValue( int index ){ return ( m_Registry.getAttValue( index ) ); }

    void  setSLevelCorrection( float dBValue ){ m_SLevelCorrection=dBValue; }
    float getSLevelCorrection(){ return m_SLevelCorrection; }

    void  setANF( double value );
    void  setDNR( double value );

    AudioQueue* getFFTQueueRF();
    AudioQueue* getFFTQueueAF();

    float       getSignalLevel();
    float       getInputLevel();
    float       getOutputLevel();
    float       getSquelchLevel();
    void        setSquelchLevel(float level);
    void        setFilter( float bandwidth );

    void        setSampleRate( double frequency ){m_Registry.setSampleRate(frequency);}
    double      getSampleRate(){return ( m_Registry.getSampleRate() );}

    void        setSampleRatePPM( double ppm ){ m_Registry.setSampleRatePPM( ppm ); }
    double      getSampleRatePPM( ){ return ( m_Registry.getSampleRatePPM() ); }

    void        setXTalPPM( double ppm ){ m_Registry.setXTalPPM( ppm ); }
    double      getXTalPPM( ){ return ( m_Registry.getXTalPPM() ); }

    void        setCurrentAttenuator( int index ){ m_Registry.setCurrentAttenuator(index); }
    int         getCurrentAttenuator(){ return ( m_Registry.getCurrentAttenuator() ); }

    void        setCurrentMode( int index ){ m_Registry.setCurrentMode(index); }
    int         getCurrentMode(){ return ( m_Registry.getCurrentMode() ); }

    void        setCurrentLOFrequency( double frequency ){ m_Registry.setCurrentLOFrequency(frequency); }
    int         getCurrentLOFrequency(){ return ( m_Registry.getCurrentLOFrequency() ); }

    void        setCurrentVFOFrequency( double frequency ){ m_Registry.setCurrentVFOFrequency(frequency); }
    int         getCurrentVFOFrequency(){ return ( m_Registry.getCurrentVFOFrequency() ); }

    void        Log( const char* format, ... )
    {
        size_t  n = 2048;
        char    buffer[2049];
        va_list va;
        va_start(va, format);
        vsnprintf( buffer, n, format, va );
        va_end(va);
        if( m_LoggingWindow )
        {
            m_LoggingWindow->logMessage( std::string( buffer ) );
        }
    }

    private:

    // S-Meter-Correction

    double                      m_SampleRate;

    // List of Output-Devices
    std::vector <AudioDevice_t> m_AudioOutputDevices;
    int                         m_OutputDeviceIndex;

    // List of Input-Devices
    std::vector <AudioDevice_t> m_AudioInputDevices;
    int                         m_InputDeviceIndex;

    bool                        m_AudioIsConfigured;

    AudioThread*  m_AudioThread;

    float                       m_SLevelCorrection;

    wxMutex                     m_ConfigMutex;

    GlobalLogging*              m_LoggingWindow;
    ConfigRegistry              m_Registry;
};

#endif
