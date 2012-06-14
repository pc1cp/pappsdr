#include "spinctrl.hpp"

const wxEventType wxEVT_CUSTOM_SPINCTRL = wxNewEventType();

enum
{
    ID_SPIN_UP,
    ID_SPIN_DOWN
};

wxCustomSpinCtrl::wxCustomSpinCtrl( wxWindow*         parent,
                                    wxWindowID        id,
                                    float             min,
                                    float             max,
                                    float             initial,
                                    float             increment,
                                    int               digits )
    :
    wxPanel    ( parent    ),
    m_Parent   ( parent    ),
    m_ID       ( id        ),
    m_Value    ( initial   ),
    m_MaxValue ( max       ),
    m_MinValue ( min       ),
    m_Increment( increment ),
    m_Digits   ( digits    )
{
    m_TextControl = new wxTextCtrl( this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );

    m_SpinButton  = new wxSpinButton( this, wxID_ANY );
    m_SpinButton->SetRange( -100000, +100000 );
    m_SpinButton->SetValue( 0 );

    wxBoxSizer*  topSizer = new wxBoxSizer( wxHORIZONTAL );
    topSizer->Add( m_TextControl, 0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );
    topSizer->Add( m_SpinButton,  0, wxALIGN_CENTER|wxALIGN_CENTER_VERTICAL );

    SetSizerAndFit( topSizer );

    printValue( m_Value );
}

wxCustomSpinCtrl::~wxCustomSpinCtrl()
{
}

void wxCustomSpinCtrl::setValue( float value )
{
    if( value > m_MaxValue )
    {
        value = m_MaxValue;
    }
    else
    if( value < m_MinValue )
    {
        value = m_MinValue;
    }

    m_Value = value;
    printValue( m_Value );
}

float wxCustomSpinCtrl::getValue()
{
    return( m_Value );
}

void wxCustomSpinCtrl::printValue( float value )
{
    wxString valueStr;
    switch( m_Digits )
    {
        case 0 :
            valueStr.Printf( _("%0.0f"), value );
            break;
        case 1 :
            valueStr.Printf( _("%0.1f"), value );
            break;
        case 2 :
            valueStr.Printf( _("%0.2f"), value );
            break;
        case 3 :
            valueStr.Printf( _("%0.3f"), value );
            break;
        case 4 :
            valueStr.Printf( _("%0.4f"), value );
            break;
        case 5 :
            valueStr.Printf( _("%0.5f"), value );
            break;
        case 6 :
            valueStr.Printf( _("%0.6f"), value );
            break;
        case 7 :
            valueStr.Printf( _("%0.7f"), value );
            break;
        case 8 :
            valueStr.Printf( _("%0.8f"), value );
            break;
        case 9 :
            valueStr.Printf( _("%0.9f"), value );
            break;
        default:
            valueStr.Printf( _("%f"), value );
            break;
    }
    m_TextControl->SetValue( valueStr );
}

void wxCustomSpinCtrl::onSpinUp( wxSpinEvent& WXUNUSED(event) )
{
    m_Value += m_Increment;
    if( m_Value > m_MaxValue )
    {
        m_Value = m_MaxValue;
    }

    printValue( m_Value );

    wxSpinEvent event;
    event.SetId( m_ID );
    m_Parent->GetEventHandler()->AddPendingEvent( event );
    std::cerr << "wxSpinEvent sent.\n";

    wxCommandEvent cmdEvent(wxEVT_CUSTOM_SPINCTRL);
    cmdEvent.SetId( m_ID );
    m_Parent->GetEventHandler()->AddPendingEvent( cmdEvent );
}

void wxCustomSpinCtrl::onSpinDown( wxSpinEvent& WXUNUSED(event) )
{
    m_Value -= m_Increment;
    if( m_Value < m_MinValue )
    {
        m_Value = m_MinValue;
    }

    printValue( m_Value );

    wxSpinEvent event;
    event.SetId( m_ID );
    m_Parent->GetEventHandler()->AddPendingEvent( event );

    wxCommandEvent cmdEvent(wxEVT_CUSTOM_SPINCTRL);
    cmdEvent.SetId( m_ID );
    m_Parent->GetEventHandler()->AddPendingEvent( cmdEvent );

    std::cerr << "wxSpinEvent sent.\n";
}

void wxCustomSpinCtrl::onTextEnter( wxCommandEvent& WXUNUSED( event ) )
{
    double number;
    if( m_TextControl->GetValue().ToDouble(&number) )
    {
        m_Value = number;

        wxSpinEvent event;
        event.SetId( m_ID );
        m_Parent->GetEventHandler()->AddPendingEvent( event );

        wxCommandEvent cmdEvent(wxEVT_CUSTOM_SPINCTRL);
        cmdEvent.SetId( m_ID );
        m_Parent->GetEventHandler()->AddPendingEvent( cmdEvent );
    }
    printValue( m_Value );
}

BEGIN_EVENT_TABLE( wxCustomSpinCtrl, wxPanel )
    EVT_SPIN_UP  ( wxID_ANY, wxCustomSpinCtrl::onSpinUp   )
    EVT_SPIN_DOWN( wxID_ANY, wxCustomSpinCtrl::onSpinDown )
    EVT_TEXT_ENTER( wxID_ANY, wxCustomSpinCtrl::onTextEnter )
END_EVENT_TABLE()

