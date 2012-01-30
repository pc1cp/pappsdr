
#include "sounddevicedialog.hpp"

MySoundDeviceDialog::MySoundDeviceDialog(wxWindow *parent)
                 : wxDialog(parent, wxID_ANY, wxString(_T("Audio I/O Selection")))
{
    GlobalConfig* Config=GlobalConfig::getInstance();

    wxBoxSizer* TopSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* OutputSizer = new wxStaticBoxSizer( wxVERTICAL,
                                                  this,
                                                  wxT("Master-Output-Device"));

    wxStaticBoxSizer* InputSizer = new wxStaticBoxSizer( wxVERTICAL,
                                                  this,
                                                  wxT("Master-Input-Device"));

    std::vector <AudioDevice_t> InDevices  = Config->getInputDevices ();
    std::vector <AudioDevice_t> OutDevices = Config->getOutputDevices();

    wxArrayString InChoices;
    wxArrayString OutChoices;

    for(unsigned int n=0; n<InDevices.size(); n++)
    {
        InChoices.Add(InDevices[n].Name);
    }

    for(unsigned int n=0; n<OutDevices.size(); n++)
    {
        OutChoices.Add(OutDevices[n].Name);
    }

    m_OutputChoice = new wxChoice(    this,
                                    SND_OUT_CHOICE,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    OutChoices,
                                    0,
                                    wxDefaultValidator,
                                    wxT("Output-Choice") );

    m_InputChoice = new wxChoice(    this,
                                    SND_IN_CHOICE,
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    InChoices,
                                    0,
                                    wxDefaultValidator,
                                    wxT("Input0-Choice") );

    m_OutputChoice->SetSelection( Config->getOutputDevice() );
    m_InputChoice->SetSelection( Config->getInputDevice () );

    OutputSizer->Add( m_OutputChoice, 0, wxALIGN_CENTER | wxALL, 5);
    InputSizer->Add( m_InputChoice, 0, wxALIGN_CENTER | wxALL, 5);

    TopSizer->Add(OutputSizer, 0, wxALIGN_CENTER | wxALL, 5);
    TopSizer->Add(InputSizer, 0, wxALIGN_CENTER | wxALL, 5);

    wxButton *btnOk = new wxButton(this, wxID_CLOSE, _T("CLOSE"));
    TopSizer->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizer(TopSizer);

    TopSizer->SetSizeHints(this);
    TopSizer->Fit(this);

    btnOk->SetFocus();
    btnOk->SetDefault();
}

void MySoundDeviceDialog::OnButtonPressed( wxCommandEvent& event )
{
    if( event.GetId() == wxID_CLOSE )
    {
        this->EndModal(wxID_CLOSE);
    }
}

void MySoundDeviceDialog::OnDeviceChanged( wxCommandEvent& event )
{
    GlobalConfig* Config=GlobalConfig::getInstance();

    if( event.GetId() == SND_OUT_CHOICE )
    {
        Config->setOutputDevice( event.GetInt() );
    }
    else
    if( event.GetId() == SND_IN_CHOICE )
    {
        Config->setInputDevice( event.GetInt() );
    }
}

BEGIN_EVENT_TABLE(MySoundDeviceDialog, wxDialog)
    EVT_BUTTON(wxID_ANY, MySoundDeviceDialog::OnButtonPressed)
    EVT_CHOICE(wxID_ANY, MySoundDeviceDialog::OnDeviceChanged)
END_EVENT_TABLE()


