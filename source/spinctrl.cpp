#include "spinctrl.hpp"

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
                                    int               digits,
                                    const wxString&   name )
    :
    wxPanel    ( parent    ),
    m_Parent   ( parent    ),
    m_ID       ( id        ),
    m_Value    ( initial   ),
    m_MinValue ( min       ),
    m_MaxValue ( max       ),
    m_Increment( increment ),
    m_Digits   ( digits    )
{
    m_TextControl = new wxTextCtrl( this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_SpinButton  = new wxSpinButton( this, wxID_ANY );

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
    value > m_MaxValue? m_MaxValue:value;
    value < m_MinValue? m_MinValue:value;
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
            valueStr.Printf( _("%0.0f"), m_Value );
            break;
        case 1 :
            valueStr.Printf( _("%0.1f"), m_Value );
            break;
        case 2 :
            valueStr.Printf( _("%0.2f"), m_Value );
            break;
        case 3 :
            valueStr.Printf( _("%0.3f"), m_Value );
            break;
        case 4 :
            valueStr.Printf( _("%0.4f"), m_Value );
            break;
        case 5 :
            valueStr.Printf( _("%0.5f"), m_Value );
            break;
        case 6 :
            valueStr.Printf( _("%0.6f"), m_Value );
            break;
        case 7 :
            valueStr.Printf( _("%0.7f"), m_Value );
            break;
        case 8 :
            valueStr.Printf( _("%0.8f"), m_Value );
            break;
        case 9 :
            valueStr.Printf( _("%0.9f"), m_Value );
            break;
        default:
            valueStr.Printf( _("%f"), m_Value );
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
    event.SetEventType( wxEVT_SPIN );
    m_Parent->GetEventHandler()->AddPendingEvent( event );
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
    event.SetEventType( wxEVT_SPIN );
    m_Parent->GetEventHandler()->AddPendingEvent( event );
}

void wxCustomSpinCtrl::onTextEnter( wxCommandEvent& WXUNUSED( event ) )
{
    double number;
    if( m_TextControl->GetValue().ToDouble(&number) )
    {
        m_Value = number;

        wxSpinEvent event;
        event.SetId( m_ID );
        event.SetEventType( wxEVT_SPIN );
        m_Parent->GetEventHandler()->AddPendingEvent( event );
    }
    printValue( m_Value );
}

BEGIN_EVENT_TABLE( wxCustomSpinCtrl, wxPanel )
    EVT_SPIN_UP  ( wxID_ANY, wxCustomSpinCtrl::onSpinUp   )
    EVT_SPIN_DOWN( wxID_ANY, wxCustomSpinCtrl::onSpinDown )
    EVT_TEXT_ENTER( wxID_ANY, wxCustomSpinCtrl::onTextEnter )
END_EVENT_TABLE()

