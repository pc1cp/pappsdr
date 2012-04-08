#include "configdialog.hpp"
#include "config.hpp"

enum 
{
    ID_CONF_INPUT_DEVICE,
    ID_CONF_OUTPUT_DEVICE,
    ID_CONF_SAMPLERATE,
    ID_CONF_SAMPLERATE_ENTER,
    ID_CONF_SAMPLERATE_PPM,
    ID_CONF_RESET
};

wxCustomConfigDialog::wxCustomConfigDialog( wxWindow* parent )
    :
    wxDialog( parent, 
              -1, 
              _("Configuration"), 
              wxDefaultPosition, 
              wxDefaultSize, 
              wxDEFAULT_DIALOG_STYLE ),
    m_Parent( parent )
{
    wxBoxSizer* mainSizer   = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* buttonSizer = new wxBoxSizer( wxHORIZONTAL );

    m_ButtonOK      = new wxButton( this, wxID_OK    , _("OK")     );
    m_ButtonCancel  = new wxButton( this, wxID_CANCEL, _("Cancel") );
    m_ButtonDefault = new wxButton( this, wxID_ANY   , _("Reset to Defaults") );


    m_Notebook = new wxNotebook( this, 
                                 wxID_ANY, 
                                 wxDefaultPosition, 
                                 wxDefaultSize, //Size(500, 500), 
                                 wxNB_MULTILINE );

    // -------------------------------------------------------------------------
    // AudioIO-Settings-Page
    // -------------------------------------------------------------------------

    m_AudioIOPanel = createAudioIOPage( m_Notebook );

    // -------------------------------------------------------------------------
    // Hardware-Settings-Page
    // -------------------------------------------------------------------------

    m_HardwarePanel = new wxPanel( m_Notebook );

    // -------------------------------------------------------------------------
    // Receiver-Settings-Page
    // -------------------------------------------------------------------------

    m_ReceiverPanel = new wxPanel( m_Notebook );

    // -------------------------------------------------------------------------
    // Add pages to notebook
    // -------------------------------------------------------------------------

    m_Notebook->AddPage( m_AudioIOPanel,  _("AudioIO") );
    m_Notebook->AddPage( m_HardwarePanel, _("Hardware") );
    m_Notebook->AddPage( m_ReceiverPanel, _("Receiver") );

    mainSizer->Add( m_Notebook,        0, wxEXPAND|wxALL, 4 );

    buttonSizer->Add( m_ButtonDefault, 0, wxEXPAND|wxALL, 4 );
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add( m_ButtonCancel,  0, wxEXPAND|wxALL, 4 );
    buttonSizer->Add( m_ButtonOK,      0, wxEXPAND|wxALL, 4 );

    mainSizer->Add( buttonSizer, 0, wxEXPAND );

    SetSizerAndFit( mainSizer );
    return;
}

wxCustomConfigDialog::~wxCustomConfigDialog()
{
}

wxPanel* wxCustomConfigDialog::createAudioIOPage( wxBookCtrlBase* parent )
{
    GlobalConfig* Config=GlobalConfig::getInstance();

    std::vector <AudioDevice_t> inDevices  = Config->getInputDevices ();
    std::vector <AudioDevice_t> outDevices = Config->getOutputDevices();
    wxArrayString inputDeviceChoices;
    wxArrayString outputDeviceChoices;

    for(unsigned int n=0; n<inDevices.size(); n++)
    {
        inputDeviceChoices.Add(inDevices[n].Name);
    }

    for(unsigned int n=0; n<outDevices.size(); n++)
    {
        outputDeviceChoices.Add(outDevices[n].Name);
    }

    wxPanel* page = new wxPanel( parent );

    wxBoxSizer* pageSizer = new wxBoxSizer( wxVERTICAL );

    wxStaticBoxSizer* inputSizer = 
        new wxStaticBoxSizer( wxVERTICAL, page, _("Input-Device") );

    wxStaticBoxSizer* outputSizer = 
        new wxStaticBoxSizer( wxVERTICAL, page, _("Output-Device") );

    wxStaticBoxSizer* samplerateSizer = 
        new wxStaticBoxSizer( wxVERTICAL, page, _("Samplerate") );

	wxChoice* inputDeviceChoice = new wxChoice( page, ID_CONF_INPUT_DEVICE, wxDefaultPosition, wxDefaultSize, inputDeviceChoices );
    inputDeviceChoice->SetSelection( Config->getInputDevice() );

	wxChoice* outputDeviceChoice = new wxChoice( page, ID_CONF_OUTPUT_DEVICE, wxDefaultPosition, wxDefaultSize, outputDeviceChoices );
    outputDeviceChoice->SetSelection( Config->getOutputDevice() );

	wxArrayString samplerateChoices;
	samplerateChoices.Add( _("44100 Samples/Second") );
	samplerateChoices.Add( _("48000 Samples/Second") );
	samplerateChoices.Add( _("88200 Samples/Second") );
	samplerateChoices.Add( _("96000 Samples/Second") );
	samplerateChoices.Add( _("176400 Samples/Second") );
	samplerateChoices.Add( _("192000 Samples/Second") );
	
	m_SampleRateChoice = new wxChoice  ( page, ID_CONF_SAMPLERATE, wxDefaultPosition, wxDefaultSize, samplerateChoices );
    m_SampleRateSpin = new wxCustomSpinCtrl( page, ID_CONF_SAMPLERATE_ENTER, 0, 200000, 0, 1 );
    m_SampleRatePPMSpin = new wxCustomSpinCtrl( page, ID_CONF_SAMPLERATE_PPM, -5000, +5000, 0, 1 );

    double sampleRate = Config->getSampleRate();
    double ppm        = Config->getSampleRatePPM();

    m_SampleRateSpin   ->setValue(sampleRate*(1.0+ppm/1.0E6));
    m_SampleRatePPMSpin->setValue(ppm);

    switch( (int)sampleRate )
    {
        default:
        case 44100:
        {
            m_SampleRateChoice->SetSelection( 0 );
            break;
        }
        case 48000:
        {
            m_SampleRateChoice->SetSelection( 1 );
            break;
        }
        case 88200:
        {
            m_SampleRateChoice->SetSelection( 2 );
            break;
        }
        case 96000:
        {
            m_SampleRateChoice->SetSelection( 3 );
            break;
        }
        case 176400:
        {
            m_SampleRateChoice->SetSelection( 4 );
            break;
        }
        case 192000:
        {
            m_SampleRateChoice->SetSelection( 5 );
            break;
        }
    }

    wxBoxSizer* sampleRateCorrectionSizer = new wxBoxSizer( wxHORIZONTAL );

    sampleRateCorrectionSizer->AddStretchSpacer();
    sampleRateCorrectionSizer->Add( new wxStaticText( page, wxID_ANY, _("FS-Deviation (ppm)") ), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
    sampleRateCorrectionSizer->Add( m_SampleRatePPMSpin );
    sampleRateCorrectionSizer->AddStretchSpacer();
    sampleRateCorrectionSizer->Add( new wxStaticText( page, wxID_ANY, _("FS-Real ") ), 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
    sampleRateCorrectionSizer->Add( m_SampleRateSpin );
    sampleRateCorrectionSizer->AddStretchSpacer();

    inputSizer->Add     ( inputDeviceChoice,         0, wxEXPAND|wxALL, 8 );
    outputSizer->Add    ( outputDeviceChoice,        0, wxEXPAND|wxALL, 8 );
    samplerateSizer->Add( m_SampleRateChoice,        0, wxEXPAND|wxALL, 8 );
    samplerateSizer->Add( sampleRateCorrectionSizer, 0, wxEXPAND|wxALL, 8 );

    wxStaticText* hintText = new wxStaticText( page, -1, 
        _( "When choosing audio-devices, your should be aware that it is "
           "absolutely required to choose devices from the same hardware-"
           "device and from the same host-API. "
           "If you don't, expect the audio-IO to close immediately after "
           "it has been started. If this happens it can be restarted "
           "after choosing other settings.") );
    hintText->Wrap( 500 );

    pageSizer->Add( inputSizer,      0, wxEXPAND|wxALL, 8);
    pageSizer->Add( outputSizer,     0, wxEXPAND|wxALL, 8);
    pageSizer->Add( hintText,        0, wxEXPAND|wxALL, 8);
    pageSizer->Add( samplerateSizer, 1, wxEXPAND|wxALL, 8);

    page->SetSizerAndFit( pageSizer );
    return( page );
}

void wxCustomConfigDialog::onInputDevice( wxCommandEvent& event )
{
    GlobalConfig* config=GlobalConfig::getInstance();
    config->setInputDevice( event.GetInt() );
}

void wxCustomConfigDialog::onOutputDevice( wxCommandEvent& event )
{
    GlobalConfig* config=GlobalConfig::getInstance();
    config->setOutputDevice( event.GetInt() );
}

void wxCustomConfigDialog::onSampleRate( wxCommandEvent& event )
{
    GlobalConfig* config=GlobalConfig::getInstance();

    float ppmFactor  = m_SampleRatePPMSpin->getValue();
          ppmFactor /= 1000000.0f;
          ppmFactor += 1.0;

    switch( event.GetInt() )
    {
        case 0:    
            config->setSampleRate( 44100.0 );
            m_SampleRateSpin->setValue( 44100.0 * ppmFactor );
            break;
        case 1:    
            config->setSampleRate( 48000.0 );
            m_SampleRateSpin->setValue( 48000.0 * ppmFactor );
            break;
        case 2:    
            config->setSampleRate( 88200.0 );
            m_SampleRateSpin->setValue( 88200.0 * ppmFactor );
            break;
        case 3:    
            config->setSampleRate( 96000.0 );
            m_SampleRateSpin->setValue( 96000.0 * ppmFactor );
            break;
        case 4:    
            config->setSampleRate( 176400.0 );
            m_SampleRateSpin->setValue( 176400.0 * ppmFactor );
            break;
        case 5:
        default:
            config->setSampleRate( 192000.0 );
            m_SampleRateSpin->setValue( 192000.0 * ppmFactor );
            break;
   }
}

void wxCustomConfigDialog::onSampleRateReal( wxSpinEvent& WXUNUSED(event) )
{
    GlobalConfig* config=GlobalConfig::getInstance();

    int sampleRateIndex = m_SampleRateChoice->GetSelection();

    double sampleRate = 0;
    switch( sampleRateIndex )
    {
        case 0:
            sampleRate = 44100.0;
            break;
        case 1:    
            sampleRate = 48000.0;
            break;
        case 2:    
            sampleRate = 88200.0;
            break;
        case 3:    
            sampleRate = 96000.0;
            break;
        case 4:    
            sampleRate = 176400.0;
            break;
        case 5:
        default:
            sampleRate = 192000.0;
            break;
   }

   double sampleRateReal = m_SampleRateSpin->getValue();
   double ppm = ((sampleRateReal / sampleRate )-1.0)*1000000.0;

   m_SampleRatePPMSpin->setValue( ppm );
   config->setSampleRatePPM( ppm );
}

void wxCustomConfigDialog::onSampleRatePPM( wxSpinEvent& WXUNUSED(event) )
{
    GlobalConfig* config=GlobalConfig::getInstance();

    int sampleRateIndex = m_SampleRateChoice->GetSelection();

    double sampleRate = 0;
    switch( sampleRateIndex )
    {
        case 0:
            sampleRate = 44100.0;
            break;
        case 1:    
            sampleRate = 48000.0;
            break;
        case 2:    
            sampleRate = 88200.0;
            break;
        case 3:    
            sampleRate = 96000.0;
            break;
        case 4:    
            sampleRate = 176400.0;
            break;
        case 5:
        default:
            sampleRate = 192000.0;
            break;
    }

    double ppm = m_SampleRatePPMSpin->getValue();
    double sampleRateReal = sampleRate*(1.0+ppm/1.0E6);
    m_SampleRateSpin->setValue( sampleRateReal );
}

BEGIN_EVENT_TABLE(wxCustomConfigDialog, wxDialog)
    EVT_CHOICE(ID_CONF_INPUT_DEVICE,     wxCustomConfigDialog::onInputDevice    )
    EVT_CHOICE(ID_CONF_OUTPUT_DEVICE,    wxCustomConfigDialog::onOutputDevice   )
    EVT_CHOICE(ID_CONF_SAMPLERATE,       wxCustomConfigDialog::onSampleRate     )
    EVT_SPIN  (ID_CONF_SAMPLERATE_ENTER, wxCustomConfigDialog::onSampleRateReal )
    EVT_SPIN  (ID_CONF_SAMPLERATE_PPM  , wxCustomConfigDialog::onSampleRatePPM  )
END_EVENT_TABLE()


