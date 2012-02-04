#include "main.hpp"

class MyApp : public wxApp
{
    public:
    virtual bool OnInit();
    Pappradio* m_Pappradio;
};

class MyFrame : public wxFrame
{
    public:
    MyFrame(const wxString& title, Pappradio* pappradio );

    void OnLoFreqChanged( wxCommandEvent& event );
    void OnFreqChanged( wxCommandEvent& event );
    void onFFTClicked ( wxCommandEvent& event );

    void onButtonCW      ( wxCommandEvent& event );
    void onButtonLSB     ( wxCommandEvent& event );
    void onButtonUSB     ( wxCommandEvent& event );
    void onButtonAM      ( wxCommandEvent& event );
    void onButtonFM      ( wxCommandEvent& event );

    void onButtonAtt00   ( wxCommandEvent& event );
    void onButtonAtt10   ( wxCommandEvent& event );
    void onButtonAtt20   ( wxCommandEvent& event );
    void onButtonAtt30   ( wxCommandEvent& event );

    void onButtonAGC4000 ( wxCommandEvent& event );
    void onButtonAGC2000 ( wxCommandEvent& event );
    void onButtonAGC1000 ( wxCommandEvent& event );
    void onButtonAGC500  ( wxCommandEvent& event );
    void onButtonAGC250  ( wxCommandEvent& event );

    void onButtonFlt0500 ( wxCommandEvent& event );
    void onButtonFlt1000 ( wxCommandEvent& event );
    void onButtonFlt1500 ( wxCommandEvent& event );
    void onButtonFlt2000 ( wxCommandEvent& event );
    void onButtonFlt2500 ( wxCommandEvent& event );
    void onButtonFlt3000 ( wxCommandEvent& event );
    void onButtonFlt3500 ( wxCommandEvent& event );
    void onButtonFlt4000 ( wxCommandEvent& event );
    void onButtonFlt4500 ( wxCommandEvent& event );
    void onButtonFlt5000 ( wxCommandEvent& event );
    void onButtonFlt5500 ( wxCommandEvent& event );
    void onButtonFlt6000 ( wxCommandEvent& event );

    void onButtonPreOff     ( wxCommandEvent& event );
    void onButtonPre2400    ( wxCommandEvent& event );
    void onButtonPre4000    ( wxCommandEvent& event );
    void onButtonPre7500    ( wxCommandEvent& event );
    void onButtonPre14000   ( wxCommandEvent& event );
    void onButtonPreCustom1 ( wxCommandEvent& event );
    void onButtonPreCustom2 ( wxCommandEvent& event );
    void onButtonPreCustom3 ( wxCommandEvent& event );

    private:
    wxCustomLCDisplay*  m_LCDisplay;
    wxCustomFFTDisplay* m_FFTDisplayRF;
    wxCustomFFTDisplay* m_FFTDisplayAF;

    wxCustomSeparator*  m_ModeSeparator;

    wxCustomPushButton* m_PushButtonCW;
    wxCustomPushButton* m_PushButtonLSB;
    wxCustomPushButton* m_PushButtonUSB;
    wxCustomPushButton* m_PushButtonAM;
    wxCustomPushButton* m_PushButtonFM;

    wxCustomSeparator*  m_AttSeparator;

    wxCustomPushButton* m_PushButtonATT00;
    wxCustomPushButton* m_PushButtonATT10;
    wxCustomPushButton* m_PushButtonATT20;
    wxCustomPushButton* m_PushButtonATT30;

    wxCustomSeparator*  m_FltSeparator;

    wxCustomPushButton* m_PushButtonFlt0500;
    wxCustomPushButton* m_PushButtonFlt1000;
    wxCustomPushButton* m_PushButtonFlt1500;
    wxCustomPushButton* m_PushButtonFlt2000;
    wxCustomPushButton* m_PushButtonFlt2500;
    wxCustomPushButton* m_PushButtonFlt3000;
    wxCustomPushButton* m_PushButtonFlt3500;
    wxCustomPushButton* m_PushButtonFlt4000;
    wxCustomPushButton* m_PushButtonFlt4500;
    wxCustomPushButton* m_PushButtonFlt5000;
    wxCustomPushButton* m_PushButtonFlt5500;
    wxCustomPushButton* m_PushButtonFlt6000;

    wxCustomSeparator*  m_PreSeparator;

    wxCustomPushButton* m_PushButtonPreOff;
    wxCustomPushButton* m_PushButtonPre2400;
    wxCustomPushButton* m_PushButtonPre4000;
    wxCustomPushButton* m_PushButtonPre7500;
    wxCustomPushButton* m_PushButtonPre14000;
    wxCustomPushButton* m_PushButtonPreCust1;
    wxCustomPushButton* m_PushButtonPreCust2;
    wxCustomPushButton* m_PushButtonPreCust3;

    wxCustomSeparator*  m_AGCSeparator;

    wxCustomPushButton* m_PushButtonAGC4000;
    wxCustomPushButton* m_PushButtonAGC2000;
    wxCustomPushButton* m_PushButtonAGC1000;
    wxCustomPushButton* m_PushButtonAGC500;
    wxCustomPushButton* m_PushButtonAGC250;

    Pappradio*          m_Pappradio;
    AudioThread*        m_AudioThread;
    DECLARE_EVENT_TABLE()
};

enum
{
    BUTTON_CW,
    BUTTON_LSB,
    BUTTON_USB,
    BUTTON_AM,
    BUTTON_FM,

    BUTTON_ATT00,
    BUTTON_ATT10,
    BUTTON_ATT20,
    BUTTON_ATT30,

    BUTTON_AGC250,
    BUTTON_AGC500,
    BUTTON_AGC1000,
    BUTTON_AGC2000,
    BUTTON_AGC4000,

    BUTTON_FLT0500,
    BUTTON_FLT1000,
    BUTTON_FLT1500,
    BUTTON_FLT2000,
    BUTTON_FLT2500,
    BUTTON_FLT3000,
    BUTTON_FLT3500,
    BUTTON_FLT4000,
    BUTTON_FLT4500,
    BUTTON_FLT5000,
    BUTTON_FLT5500,
    BUTTON_FLT6000,

    BUTTON_PRE_OFF,
    BUTTON_PRE_2400,
    BUTTON_PRE_4000,
    BUTTON_PRE_7500,
    BUTTON_PRE_14000,
    BUTTON_PRE_CUSTOM1,
    BUTTON_PRE_CUSTOM2,
    BUTTON_PRE_CUSTOM3,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_COMMAND(wxID_ANY, wxLOFreqChanged,  MyFrame::OnLoFreqChanged )
    EVT_COMMAND(wxID_ANY, wxFreqChanged,    MyFrame::OnFreqChanged   )
    EVT_COMMAND(wxID_ANY, wxFFTClicked,     MyFrame::onFFTClicked    )

    EVT_BUTTON (BUTTON_CW , MyFrame::onButtonCW  )
    EVT_BUTTON (BUTTON_LSB, MyFrame::onButtonLSB )
    EVT_BUTTON (BUTTON_USB, MyFrame::onButtonUSB )
    EVT_BUTTON (BUTTON_AM , MyFrame::onButtonAM  )
    EVT_BUTTON (BUTTON_FM , MyFrame::onButtonFM  )

    EVT_BUTTON (BUTTON_ATT00, MyFrame::onButtonAtt00 )
    EVT_BUTTON (BUTTON_ATT10, MyFrame::onButtonAtt10 )
    EVT_BUTTON (BUTTON_ATT20, MyFrame::onButtonAtt20 )
    EVT_BUTTON (BUTTON_ATT30, MyFrame::onButtonAtt30 )

    EVT_BUTTON (BUTTON_FLT0500, MyFrame::onButtonFlt0500 )
    EVT_BUTTON (BUTTON_FLT1000, MyFrame::onButtonFlt1000 )
    EVT_BUTTON (BUTTON_FLT1500, MyFrame::onButtonFlt1500 )
    EVT_BUTTON (BUTTON_FLT2000, MyFrame::onButtonFlt2000 )
    EVT_BUTTON (BUTTON_FLT2500, MyFrame::onButtonFlt2500 )
    EVT_BUTTON (BUTTON_FLT3000, MyFrame::onButtonFlt3000 )
    EVT_BUTTON (BUTTON_FLT3500, MyFrame::onButtonFlt3500 )
    EVT_BUTTON (BUTTON_FLT4000, MyFrame::onButtonFlt4000 )
    EVT_BUTTON (BUTTON_FLT4500, MyFrame::onButtonFlt4500 )
    EVT_BUTTON (BUTTON_FLT5000, MyFrame::onButtonFlt5000 )
    EVT_BUTTON (BUTTON_FLT5500, MyFrame::onButtonFlt5500 )
    EVT_BUTTON (BUTTON_FLT6000, MyFrame::onButtonFlt6000 )

    EVT_BUTTON (BUTTON_AGC4000, MyFrame::onButtonAGC4000 )
    EVT_BUTTON (BUTTON_AGC2000, MyFrame::onButtonAGC2000 )
    EVT_BUTTON (BUTTON_AGC1000, MyFrame::onButtonAGC1000 )
    EVT_BUTTON (BUTTON_AGC500 , MyFrame::onButtonAGC500  )
    EVT_BUTTON (BUTTON_AGC250 , MyFrame::onButtonAGC250  )

    EVT_BUTTON (BUTTON_PRE_OFF    , MyFrame::onButtonPreOff     )
    EVT_BUTTON (BUTTON_PRE_2400   , MyFrame::onButtonPre2400    )
    EVT_BUTTON (BUTTON_PRE_4000   , MyFrame::onButtonPre4000    )
    EVT_BUTTON (BUTTON_PRE_7500   , MyFrame::onButtonPre7500    )
    EVT_BUTTON (BUTTON_PRE_14000  , MyFrame::onButtonPre14000   )
    EVT_BUTTON (BUTTON_PRE_CUSTOM1, MyFrame::onButtonPreCustom1 )
    EVT_BUTTON (BUTTON_PRE_CUSTOM2, MyFrame::onButtonPreCustom2 )
    EVT_BUTTON (BUTTON_PRE_CUSTOM3, MyFrame::onButtonPreCustom3 )

END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // We do not want to have only BMPs...
    wxInitAllImageHandlers();

    m_Pappradio = new Pappradio;

    GlobalConfig* config = GlobalConfig::getInstance();
    config->startAudioThread();

    MyFrame *frame = new MyFrame( wxT("Pappradio SDR"), m_Pappradio );
    frame->Show(true);

    enum Pappradio::ERRORS result;

    // try loading the fqdata.dat from various locations
    if( !m_Pappradio->loadFQData( "fqdata.dat" ) )
    {
        if( !m_Pappradio->loadFQData( "~/fqdata.dat" ) )
        {
            m_Pappradio->loadFQData( "/usr/share/PappSDR/fqdata.dat" );
        }
    }
    result = m_Pappradio->open();

    if( result != Pappradio::ERR_NONE )
    {
        switch( result )
        {
            case Pappradio::ERR_INIT_FQDATA:
            {
                std::cerr << "*** ERROR: Loading fqdata.dat failed.\n"
                          << std::endl;
                break;
            }
            case Pappradio::ERR_NONE:
            {
                std::cerr << "*** SUCCESS: Pappradio opened.\n"
                          << std::endl;
                break;
            }
        }
        return false;
    }

    m_Pappradio->setFilter( Pappradio::BP_04000_07500 );
    //m_Pappradio->setFilter( Pappradio::BP_02400_04000 );
    m_Pappradio->setAttenuator( Pappradio::ATT00 );
    //m_Pappradio->setFrequency( 7100000 );

    return true;
}

#include "wx/mstream.h"
MyFrame::MyFrame(const wxString& title, Pappradio* pappradio )
  : wxFrame(NULL, wxID_ANY, title),
  m_Pappradio( pappradio )
{
    #if defined(__WXGTK__) || defined(__WXMOTIF__)
    {
        wxMemoryInputStream image_data(Icon_png, Icon_png_size );
        wxBitmap* iconImage = new wxBitmap( wxImage( image_data ) );
        wxIcon icon;
        icon.CopyFromBitmap(*iconImage);
        SetIcon( icon );
    }
    #endif

    wxBoxSizer* BoxSizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* BoxSizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* BoxSizer3 = new wxBoxSizer( wxVERTICAL );

    m_LCDisplay = new wxCustomLCDisplay( this );
    m_FFTDisplayRF = new wxCustomFFTDisplay( this, wxCustomFFTDisplay::FFT_RADIOFREQ );
    m_FFTDisplayAF = new wxCustomFFTDisplay( this, wxCustomFFTDisplay::FFT_AUDIOFREQ );

    BoxSizer1->Add( m_FFTDisplayRF );
    BoxSizer1->Add( BoxSizer2 );
    BoxSizer3->Add( m_LCDisplay  );

    m_ModeSeparator = new wxCustomSeparator ( this, _("Mode"), 512 );
    m_PushButtonCW  = new wxCustomPushButton( this, BUTTON_CW , _("CW") , false );
    m_PushButtonLSB = new wxCustomPushButton( this, BUTTON_LSB, _("LSB"), true  );
    m_PushButtonUSB = new wxCustomPushButton( this, BUTTON_USB, _("USB"), false );
    m_PushButtonAM  = new wxCustomPushButton( this, BUTTON_AM , _("AM") , false );
    m_PushButtonFM  = new wxCustomPushButton( this, BUTTON_FM , _("FM") , false );

    wxBoxSizer* modesBox = new wxBoxSizer( wxHORIZONTAL );

    modesBox->Add( m_PushButtonCW  );
    modesBox->Add( m_PushButtonLSB );
    modesBox->Add( m_PushButtonUSB );
    modesBox->Add( m_PushButtonAM  );
    modesBox->Add( m_PushButtonFM  );

    BoxSizer3->Add( m_ModeSeparator );
    BoxSizer3->Add( modesBox );

    m_AttSeparator    = new wxCustomSeparator ( this, _("Attenuator"), 512 );

    m_PushButtonATT00 = new wxCustomPushButton( this, BUTTON_ATT00, _("Off"   ), true  );
    m_PushButtonATT10 = new wxCustomPushButton( this, BUTTON_ATT10, _("-10 dB"), false );
    m_PushButtonATT20 = new wxCustomPushButton( this, BUTTON_ATT20, _("-20 dB"), false );
    m_PushButtonATT30 = new wxCustomPushButton( this, BUTTON_ATT30, _("-30 dB"), false );

    wxBoxSizer* attenuatorBox = new wxBoxSizer( wxHORIZONTAL );

    attenuatorBox->Add( m_PushButtonATT00 );
    attenuatorBox->Add( m_PushButtonATT10 );
    attenuatorBox->Add( m_PushButtonATT20 );
    attenuatorBox->Add( m_PushButtonATT30 );

    BoxSizer3->Add( m_AttSeparator );
    BoxSizer3->Add( attenuatorBox );

    m_FltSeparator    = new wxCustomSeparator ( this, _("ZF/AF-Filter"), 512 );

    m_PushButtonFlt0500 = new wxCustomPushButton( this, BUTTON_FLT0500, _("0.5 kHz"), false );
    m_PushButtonFlt1000 = new wxCustomPushButton( this, BUTTON_FLT1000, _("1.0 kHz"), false );
    m_PushButtonFlt1500 = new wxCustomPushButton( this, BUTTON_FLT1500, _("1.5 kHz"), false );
    m_PushButtonFlt2000 = new wxCustomPushButton( this, BUTTON_FLT2000, _("2.0 kHz"), false );
    m_PushButtonFlt2500 = new wxCustomPushButton( this, BUTTON_FLT2500, _("2.5 kHz"), false );
    m_PushButtonFlt3000 = new wxCustomPushButton( this, BUTTON_FLT3000, _("3.0 kHz"), false );
    m_PushButtonFlt3500 = new wxCustomPushButton( this, BUTTON_FLT3500, _("3.5 kHz"), false );
    m_PushButtonFlt4000 = new wxCustomPushButton( this, BUTTON_FLT4000, _("4.0 kHz"), false );
    m_PushButtonFlt4500 = new wxCustomPushButton( this, BUTTON_FLT4500, _("4.5 kHz"), false );
    m_PushButtonFlt5000 = new wxCustomPushButton( this, BUTTON_FLT5000, _("5.0 kHz"), false );
    m_PushButtonFlt5500 = new wxCustomPushButton( this, BUTTON_FLT5500, _("5.5 kHz"), false );
    m_PushButtonFlt6000 = new wxCustomPushButton( this, BUTTON_FLT6000, _("6.0 kHz"), false );

    wxBoxSizer* audioBox = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* filterBox1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* filterBox2 = new wxBoxSizer( wxHORIZONTAL );

    filterBox1->Add( m_PushButtonFlt0500 );
    filterBox1->Add( m_PushButtonFlt1000 );
    filterBox1->Add( m_PushButtonFlt1500 );
    filterBox1->Add( m_PushButtonFlt2000 );
    filterBox1->Add( m_PushButtonFlt2500 );
    filterBox1->Add( m_PushButtonFlt3000 );
    filterBox1->Add( m_PushButtonFlt3500 );
    filterBox1->Add( m_PushButtonFlt4000 );
    filterBox2->Add( m_PushButtonFlt4500 );
    filterBox2->Add( m_PushButtonFlt5000 );
    filterBox2->Add( m_PushButtonFlt5500 );
    filterBox2->Add( m_PushButtonFlt6000 );

    m_AGCSeparator    = new wxCustomSeparator ( this, _("Automatic Gain Control"), 512 );

    wxBoxSizer* agcBox = new wxBoxSizer( wxHORIZONTAL );
    m_PushButtonAGC4000 = new wxCustomPushButton( this, BUTTON_AGC4000, _("4000 ms"), false );
    m_PushButtonAGC2000 = new wxCustomPushButton( this, BUTTON_AGC2000, _("2000 ms"), false );
    m_PushButtonAGC1000 = new wxCustomPushButton( this, BUTTON_AGC1000, _("1000 ms"), false );
    m_PushButtonAGC500  = new wxCustomPushButton( this, BUTTON_AGC500 , _("500 ms" ), false );
    m_PushButtonAGC250  = new wxCustomPushButton( this, BUTTON_AGC250 , _("250 ms" ), false );

    agcBox->Add( m_PushButtonAGC4000 );
    agcBox->Add( m_PushButtonAGC2000 );
    agcBox->Add( m_PushButtonAGC1000 );
    agcBox->Add( m_PushButtonAGC500 );
    agcBox->Add( m_PushButtonAGC250 );

    audioBox->Add( m_FFTDisplayAF );
    audioBox->Add( m_FltSeparator );
    audioBox->Add( filterBox1 );
    audioBox->Add( filterBox2 );
    audioBox->Add( m_AGCSeparator );
    audioBox->Add( agcBox );

    m_PreSeparator    = new wxCustomSeparator ( this, _("Preselector"), 512 );

    m_PushButtonPreOff  = new wxCustomPushButton( this, BUTTON_PRE_OFF, _("Off"), false );
    m_PushButtonPre2400 = new wxCustomPushButton( this, BUTTON_PRE_2400, _("2.4 - 4"), false );
    m_PushButtonPre4000 = new wxCustomPushButton( this, BUTTON_PRE_4000, _("4 - 7.5"), false );
    m_PushButtonPre7500 = new wxCustomPushButton( this, BUTTON_PRE_7500, _("7.5 - 14"), false );
    m_PushButtonPre14000 = new wxCustomPushButton( this, BUTTON_PRE_14000, _("14 - 30"), false );
    m_PushButtonPreCust1 = new wxCustomPushButton( this, BUTTON_PRE_CUSTOM1, _("User 1"), false );
    m_PushButtonPreCust2 = new wxCustomPushButton( this, BUTTON_PRE_CUSTOM2, _("User 2"), false );
    m_PushButtonPreCust3 = new wxCustomPushButton( this, BUTTON_PRE_CUSTOM3, _("User 3"), false );

    wxBoxSizer* preselBox = new wxBoxSizer( wxHORIZONTAL );
    preselBox->Add( m_PushButtonPreOff );
    preselBox->Add( m_PushButtonPre2400 );
    preselBox->Add( m_PushButtonPre4000 );
    preselBox->Add( m_PushButtonPre7500 );
    preselBox->Add( m_PushButtonPre14000 );
    preselBox->Add( m_PushButtonPreCust1 );
    preselBox->Add( m_PushButtonPreCust2 );
    preselBox->Add( m_PushButtonPreCust3 );

    BoxSizer3->Add( m_PreSeparator );
    BoxSizer3->Add( preselBox );

    BoxSizer2->Add( BoxSizer3 );
    BoxSizer2->Add( audioBox );

    this->SetBackgroundColour( wxColour( 33,33,33 ) );

    // -----------------------------------------------------------------------
    // set GUI and engine to sane default
    // -----------------------------------------------------------------------

    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_LSB );
    m_PushButtonCW ->setValue(false);
    m_PushButtonLSB->setValue(true );
    m_PushButtonUSB->setValue(false);
    m_PushButtonAM ->setValue(false);
    m_PushButtonFM ->setValue(false);

    config->setFilter( 2500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(true );
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);

    config->setAGCTime( 0.250, 0.250 );
    m_PushButtonAGC4000->setValue(false);
    m_PushButtonAGC2000->setValue(false);
    m_PushButtonAGC1000->setValue(false);
    m_PushButtonAGC500 ->setValue(false);
    m_PushButtonAGC250 ->setValue(true );

    SetSizerAndFit(BoxSizer1);
    Layout();

    config->setSLevelCorrection( -30.0 );
}

void MyFrame::onButtonCW(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_CW );
    m_PushButtonCW ->setValue(true );
    m_PushButtonLSB->setValue(false);
    m_PushButtonUSB->setValue(false);
    m_PushButtonAM ->setValue(false);
    m_PushButtonFM ->setValue(false);

    config->setFilter( 1000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(true );
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonLSB(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_LSB );
    m_PushButtonCW ->setValue(false);
    m_PushButtonLSB->setValue(true );
    m_PushButtonUSB->setValue(false);
    m_PushButtonAM ->setValue(false);
    m_PushButtonFM ->setValue(false);

    config->setFilter( 2500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(true );
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonUSB(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_USB );
    m_PushButtonCW ->setValue(false);
    m_PushButtonLSB->setValue(false);
    m_PushButtonUSB->setValue(true );
    m_PushButtonAM ->setValue(false);
    m_PushButtonFM ->setValue(false);

    config->setFilter( 2500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(true );
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonAM(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_AM );
    m_PushButtonCW ->setValue(false);
    m_PushButtonLSB->setValue(false);
    m_PushButtonUSB->setValue(false);
    m_PushButtonAM ->setValue(true );
    m_PushButtonFM ->setValue(false);

    config->setFilter( 4500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(true );
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFM(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();

    config->setMode( SDRAudio::SDR_MODE_FM );
    m_PushButtonCW ->setValue(false);
    m_PushButtonLSB->setValue(false);
    m_PushButtonUSB->setValue(false);
    m_PushButtonAM ->setValue(false);
    m_PushButtonFM ->setValue(true );

    config->setFilter( 2500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(true );
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonAtt00(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setATTdB( 0 );

    m_Pappradio->setAttenuator( Pappradio::ATT00 );
    m_PushButtonATT00->setValue(true );
    m_PushButtonATT10->setValue(false);
    m_PushButtonATT20->setValue(false);
    m_PushButtonATT30->setValue(false);
    m_LCDisplay->setAttenuatorValue( 0.0 );
}

void MyFrame::onButtonAtt10(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setATTdB( -10 );

    m_Pappradio->setAttenuator( Pappradio::ATT10 );
    m_PushButtonATT00->setValue(false);
    m_PushButtonATT10->setValue(true );
    m_PushButtonATT20->setValue(false);
    m_PushButtonATT30->setValue(false);
    m_LCDisplay->setAttenuatorValue( -10.0 );
}

void MyFrame::onButtonAtt20(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setATTdB( -20 );

    m_Pappradio->setAttenuator( Pappradio::ATT20 );
    m_PushButtonATT00->setValue(false);
    m_PushButtonATT10->setValue(false);
    m_PushButtonATT20->setValue(true );
    m_PushButtonATT30->setValue(false);
    m_LCDisplay->setAttenuatorValue( -20.0 );
}

void MyFrame::onButtonAtt30(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setATTdB( -30 );

    m_Pappradio->setAttenuator( Pappradio::ATT30 );
    m_PushButtonATT00->setValue(false);
    m_PushButtonATT10->setValue(false);
    m_PushButtonATT20->setValue(false);
    m_PushButtonATT30->setValue(true );
    m_LCDisplay->setAttenuatorValue( -30.0 );
}

void MyFrame::onButtonFlt0500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 500 );
    m_PushButtonFlt0500->setValue(true );
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt1000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 1000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(true );
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt1500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 1500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(true );
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt2000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 2000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(true );
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt2500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 2500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(true );
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt3000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 3000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(true );
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt3500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 3500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(true );
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt4000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 4000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(true );
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt4500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 4500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(true );
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt5000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 5000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(true );
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt5500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 5500 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(true );
    m_PushButtonFlt6000->setValue(false);
}

void MyFrame::onButtonFlt6000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setFilter( 6000 );
    m_PushButtonFlt0500->setValue(false);
    m_PushButtonFlt1000->setValue(false);
    m_PushButtonFlt1500->setValue(false);
    m_PushButtonFlt2000->setValue(false);
    m_PushButtonFlt2500->setValue(false);
    m_PushButtonFlt3000->setValue(false);
    m_PushButtonFlt3500->setValue(false);
    m_PushButtonFlt4000->setValue(false);
    m_PushButtonFlt4500->setValue(false);
    m_PushButtonFlt5000->setValue(false);
    m_PushButtonFlt5500->setValue(false);
    m_PushButtonFlt6000->setValue(true );
}

void MyFrame::OnLoFreqChanged( wxCommandEvent& WXUNUSED(event) )
{
    std::cerr << "LO-Changed\n";
    std::cerr << "Target-f : " << m_LCDisplay->getLOFreq() << "\n";
    m_Pappradio->setFrequency( (double)m_LCDisplay->getLOFreq() );

    // set Filter
    if( m_Pappradio->getFrequency() <  2400000 ||
        m_Pappradio->getFrequency() > 30000000 )
    {
        m_Pappradio->setFilter( Pappradio::NO_FILTER );
        m_PushButtonPreOff  ->setValue(true );
        m_PushButtonPre2400 ->setValue(false);
        m_PushButtonPre4000 ->setValue(false);
        m_PushButtonPre7500 ->setValue(false);
        m_PushButtonPre14000->setValue(false);
        m_PushButtonPreCust1->setValue(false);
        m_PushButtonPreCust2->setValue(false);
        m_PushButtonPreCust3->setValue(false);
    }
    else
    if( m_Pappradio->getFrequency() >=  2400000 &&
        m_Pappradio->getFrequency()  <  4000000 )
    {
        m_Pappradio->setFilter( Pappradio::BP_02400_04000 );
        m_PushButtonPreOff  ->setValue(false);
        m_PushButtonPre2400 ->setValue(true );
        m_PushButtonPre4000 ->setValue(false);
        m_PushButtonPre7500 ->setValue(false);
        m_PushButtonPre14000->setValue(false);
        m_PushButtonPreCust1->setValue(false);
        m_PushButtonPreCust2->setValue(false);
        m_PushButtonPreCust3->setValue(false);
    }
    else
    if( m_Pappradio->getFrequency() >=  4000000 &&
        m_Pappradio->getFrequency()  <  7500000 )
    {
        m_Pappradio->setFilter( Pappradio::BP_04000_07500 );
        m_PushButtonPreOff  ->setValue(false);
        m_PushButtonPre2400 ->setValue(false);
        m_PushButtonPre4000 ->setValue(true );
        m_PushButtonPre7500 ->setValue(false);
        m_PushButtonPre14000->setValue(false);
        m_PushButtonPreCust1->setValue(false);
        m_PushButtonPreCust2->setValue(false);
        m_PushButtonPreCust3->setValue(false);
    }
    else
    if( m_Pappradio->getFrequency() >=  7500000 &&
        m_Pappradio->getFrequency()  < 14000000 )
    {
        m_Pappradio->setFilter( Pappradio::BP_07500_14000 );
        m_PushButtonPreOff  ->setValue(false);
        m_PushButtonPre2400 ->setValue(false);
        m_PushButtonPre4000 ->setValue(false);
        m_PushButtonPre7500 ->setValue(true );
        m_PushButtonPre14000->setValue(false);
        m_PushButtonPreCust1->setValue(false);
        m_PushButtonPreCust2->setValue(false);
        m_PushButtonPreCust3->setValue(false);
    }
    else
    if( m_Pappradio->getFrequency() >= 14000000 &&
        m_Pappradio->getFrequency()  < 30000000 )
    {
        m_Pappradio->setFilter( Pappradio::BP_14000_30000 );
        m_PushButtonPreOff  ->setValue(false);
        m_PushButtonPre2400 ->setValue(false);
        m_PushButtonPre4000 ->setValue(false);
        m_PushButtonPre7500 ->setValue(false);
        m_PushButtonPre14000->setValue(true );
        m_PushButtonPreCust1->setValue(false);
        m_PushButtonPreCust2->setValue(false);
        m_PushButtonPreCust3->setValue(false);
    }

    // update displayed LO-Frequency
    m_LCDisplay->setLOFreqDisplay( m_Pappradio->getFrequency() );

    // update displayed Tune-Frequency
    m_LCDisplay->setFreqDisplay( m_LCDisplay->getLOFreqDisplay() +
                                 m_LCDisplay->getFreq()          );

    m_FFTDisplayRF->setLO( m_LCDisplay->getLOFreqDisplay() );

    std::cerr << "Real-f : " << m_Pappradio->getFrequency() << "\n";
}

void MyFrame::OnFreqChanged( wxCommandEvent& WXUNUSED(event) )
{
    GlobalConfig* config = GlobalConfig::getInstance();

    m_LCDisplay->setFreqDisplay( m_LCDisplay->getLOFreqDisplay() +
                                 m_LCDisplay->getFreq()          );

    config->setTune( m_LCDisplay->getFreq() );
    m_FFTDisplayRF->setTune( m_LCDisplay->getFreq() );

    std::cerr << "Freq-Changed : " << m_LCDisplay->getFreq() << "\n";
}

void MyFrame::onFFTClicked( wxCommandEvent& event )
{
    GlobalConfig* config = GlobalConfig::getInstance();

    double newTuneFrequency = ((double)event.GetInt()/1024.f)*
                              config->getInputSampleRate();
    double currentLOFrequency = m_LCDisplay->getLOFreqDisplay();
    double targetFrequency = currentLOFrequency + newTuneFrequency;

    targetFrequency = round(targetFrequency/500.0)*500.0;
    targetFrequency = targetFrequency - currentLOFrequency;
    newTuneFrequency = targetFrequency;

    m_LCDisplay->setFreq( newTuneFrequency );

    m_LCDisplay->setFreqDisplay( m_LCDisplay->getLOFreqDisplay() +
                                 newTuneFrequency          );

    config->setTune( newTuneFrequency );
    m_FFTDisplayRF->setTune( newTuneFrequency );

    std::cerr << "Freq-Changed(clicked) : " << newTuneFrequency << "\n";
}

void MyFrame::onButtonAGC4000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setAGCTime( 4.000, 0.250 );
    m_PushButtonAGC4000->setValue(true );
    m_PushButtonAGC2000->setValue(false );
    m_PushButtonAGC1000->setValue(false);
    m_PushButtonAGC500 ->setValue(false);
    m_PushButtonAGC250 ->setValue(false);
}

void MyFrame::onButtonAGC2000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setAGCTime( 2.000, 0.250 );
    m_PushButtonAGC4000->setValue(false);
    m_PushButtonAGC2000->setValue(true );
    m_PushButtonAGC1000->setValue(false);
    m_PushButtonAGC500 ->setValue(false);
    m_PushButtonAGC250 ->setValue(false);
}

void MyFrame::onButtonAGC1000(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setAGCTime( 1.000, 0.250 );
    m_PushButtonAGC4000->setValue(false);
    m_PushButtonAGC2000->setValue(false);
    m_PushButtonAGC1000->setValue(true );
    m_PushButtonAGC500 ->setValue(false);
    m_PushButtonAGC250 ->setValue(false);
}

void MyFrame::onButtonAGC500(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setAGCTime( 0.500, 0.250 );
    m_PushButtonAGC4000->setValue(false);
    m_PushButtonAGC2000->setValue(false);
    m_PushButtonAGC1000->setValue(false);
    m_PushButtonAGC500 ->setValue(true );
    m_PushButtonAGC250 ->setValue(false);
}

void MyFrame::onButtonAGC250(wxCommandEvent& WXUNUSED(event))
{
    GlobalConfig* config = GlobalConfig::getInstance();
    config->setAGCTime( 0.250, 0.250 );
    m_PushButtonAGC4000->setValue(false);
    m_PushButtonAGC2000->setValue(false);
    m_PushButtonAGC1000->setValue(false);
    m_PushButtonAGC500 ->setValue(false);
    m_PushButtonAGC250 ->setValue(true );
}

void MyFrame::onButtonPreOff(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::NO_FILTER );
    m_PushButtonPreOff  ->setValue(true );
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPre2400(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::BP_02400_04000 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(true );
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPre4000(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::BP_04000_07500 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(true );
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPre7500(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::BP_07500_14000 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(true );
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPre14000(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::BP_14000_30000 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(true );
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPreCustom1(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::EXT_1 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(true );
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPreCustom2(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::EXT_2 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(true );
    m_PushButtonPreCust3->setValue(false);
}

void MyFrame::onButtonPreCustom3(wxCommandEvent& WXUNUSED(event))
{
    m_Pappradio->setFilter( Pappradio::EXT_3 );
    m_PushButtonPreOff  ->setValue(false);
    m_PushButtonPre2400 ->setValue(false);
    m_PushButtonPre4000 ->setValue(false);
    m_PushButtonPre7500 ->setValue(false);
    m_PushButtonPre14000->setValue(false);
    m_PushButtonPreCust1->setValue(false);
    m_PushButtonPreCust2->setValue(false);
    m_PushButtonPreCust3->setValue(true );
}

