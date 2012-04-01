//=============================================================================
//
// file:
//
// purpose: implementing a custom-widget-pushbutton
//
//=============================================================================

#include "pushbutton.hpp"
#include "wx/mstream.h"

DEFINE_EVENT_TYPE(wxEVT_CUSTOM_BUTTON_EVENT)

BEGIN_EVENT_TABLE(wxCustomPushButton, wxPanel)

    // catch some mouse events
    EVT_LEFT_DOWN           (wxCustomPushButton::mouseDown   )
    EVT_LEFT_DCLICK         (wxCustomPushButton::mouseDClick )
    EVT_LEFT_UP             (wxCustomPushButton::mouseUp     )
    EVT_ENTER_WINDOW        (wxCustomPushButton::mouseEnter  )
    EVT_LEAVE_WINDOW        (wxCustomPushButton::mouseLeave  )

    // catch paint events
    EVT_PAINT               (wxCustomPushButton::paintEvent  )
    EVT_ERASE_BACKGROUND    (wxCustomPushButton::onEraseEvent)

END_EVENT_TABLE()


// ============================================================================
// define static class variables
// ============================================================================

int          wxCustomPushButton::m_ReferenceCount       = 0;
wxBitmap*    wxCustomPushButton::m_ToggleButtonBitmap1  = 0;
wxBitmap*    wxCustomPushButton::m_ToggleButtonBitmap2  = 0;
wxBitmap*    wxCustomPushButton::m_ToggleButtonBitmap3  = 0;
wxBitmap*    wxCustomPushButton::m_ToggleButtonBitmap4  = 0;


//=============================================================================
// constructor
//=============================================================================

wxCustomPushButton::wxCustomPushButton( wxWindow* parent,
                                            int       ID,
                                            wxString  label,
                                            bool      initialstate )
                                            :
                                            wxPanel(parent),
                                            m_ID(ID),
                                            m_Label(label)
{
    m_Value = initialstate;

    // if not existing, create bitmap-pointer...
    if( !m_ToggleButtonBitmap1 )
    {
        wxMemoryInputStream image_data(pushbutton_png, pushbutton_png_size );
        m_ToggleButtonBitmap1 = new wxBitmap( wxImage( image_data ) );
        //m_ToggleButtonBitmap1 = new wxBitmap( wxT("../artwork/on_off_button_inactive.png"), wxBITMAP_TYPE_PNG );
    }

    // if not existing, create bitmap-pointer...
    if( !m_ToggleButtonBitmap2 )
    {
        wxMemoryInputStream image_data(pushbutton_hover_png, pushbutton_hover_png_size);
        m_ToggleButtonBitmap2 = new wxBitmap( wxImage( image_data ) );
    }

    // if not existing, create bitmap-pointer...
    if( !m_ToggleButtonBitmap3 )
    {
        wxMemoryInputStream image_data(pushbutton_active_png, pushbutton_active_png_size);
        m_ToggleButtonBitmap3 = new wxBitmap( wxImage( image_data ) );
    }

    // if not existing, create bitmap-pointer...
    if( !m_ToggleButtonBitmap4 )
    {
        wxMemoryInputStream image_data(pushbutton_active_hover_png, pushbutton_active_hover_png_size);
        m_ToggleButtonBitmap4 = new wxBitmap( wxImage( image_data ) );
    }

    // we use reference-counting for the bitmap-pointers...
    m_ReferenceCount++;

    // our widget is exactly the size of the bitmaps used...
    m_Size = wxSize( m_ToggleButtonBitmap1->GetWidth(),
                     m_ToggleButtonBitmap1->GetHeight() );
    SetMinSize( m_Size );
    SetMaxSize( m_Size );

    // aquire font of appropriate size
#ifdef _WIN32 
    m_Font = wxFont::New( wxSize( 0, 12 ), wxFONTFAMILY_SWISS );
#else
    m_Font = wxFont::New( wxSize( 0, 13 ), wxFONTFAMILY_SWISS );
#endif

    // default is not hoverd
    m_Hover = false;
}


//=============================================================================
// destructor
//=============================================================================

wxCustomPushButton::~wxCustomPushButton()
{
    m_ReferenceCount--;

    if( m_ReferenceCount == 0 )
    {
        delete m_ToggleButtonBitmap1;
        delete m_ToggleButtonBitmap2;
        delete m_ToggleButtonBitmap3;
        delete m_ToggleButtonBitmap4;
    }
}

//=============================================================================
// Paint the whole thing on screen
//=============================================================================

void wxCustomPushButton::paintNow(wxDC& dc)
{
    if( m_Value == false && m_Hover == false )
    {
        // select background bitmap into memory-DC
        m_MemoryDC.SelectObject(*m_ToggleButtonBitmap1);
    }
    else
    if( m_Value == false && m_Hover == true )
    {
        // select background bitmap into memory-DC
        m_MemoryDC.SelectObject(*m_ToggleButtonBitmap2);
    }
    else
    if( m_Value == true && m_Hover == false )
    {
        // select background bitmap into memory-DC
        m_MemoryDC.SelectObject(*m_ToggleButtonBitmap3);
    }
    else
    {
        // select background bitmap into memory-DC
        m_MemoryDC.SelectObject(*m_ToggleButtonBitmap4);
    }

    // blit it on screen
    dc.DestroyClippingRegion();
    dc.Blit( 0, 0, m_Size.x, m_Size.y, &m_MemoryDC, 0, 0, wxCOPY, false);

    wxCoord tW;
    wxCoord tH;
    dc.SetFont( *m_Font );
    dc.GetTextExtent( m_Label, &tW, &tH );

    dc.SetTextForeground( *wxBLACK );
    dc.DrawText( m_Label, (m_Size.x - tW)/2 - 1, (m_Size.y - tH)/2 + 5 );
    dc.SetTextForeground( *wxLIGHT_GREY );
    dc.DrawText( m_Label, (m_Size.x - tW)/2    , (m_Size.y - tH)/2 + 6 );

    // free memory-DC after painting (required for Win32)
    m_MemoryDC.SelectObject(wxNullBitmap);
}

//=============================================================================
// Eventhandling
//=============================================================================

void wxCustomPushButton::onEraseEvent(wxEraseEvent& WXUNUSED(event))
{
    // do nothing to avoid flicker on Win32...
}

void wxCustomPushButton::paintEvent(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    this->paintNow(dc);
}

void wxCustomPushButton::mouseEnter (wxMouseEvent& WXUNUSED(event) )
{
    m_Hover = true;
    wxClientDC dc(this);
    this->paintNow(dc);
}

void wxCustomPushButton::mouseLeave (wxMouseEvent& WXUNUSED(event) )
{
    m_Hover = false;
    wxClientDC dc(this);
    this->paintNow(dc);
}

void wxCustomPushButton::mouseDown (wxMouseEvent& WXUNUSED(event) )
{
    #if 0
    if( event.LeftDown() )
    {
        m_Value = !m_Value;

        wxCommandEvent my_event(wxEVT_COMMAND_BUTTON_CLICKED, m_ID );
        my_event.SetInt( m_Value );

        // Send it
        GetEventHandler()->ProcessEvent( my_event );

        wxClientDC dc(this);
        this->paintNow(dc);
    }
    #endif
}

void wxCustomPushButton::mouseDClick (wxMouseEvent& event)
{
    if( event.LeftDClick() )
    {
        m_Value = !m_Value;

        wxCommandEvent my_event(wxEVT_COMMAND_BUTTON_CLICKED, m_ID );
        my_event.SetInt( m_Value );

        // Send it
        GetEventHandler()->ProcessEvent( my_event );

        wxClientDC dc(this);
        this->paintNow(dc);
    }
}

void wxCustomPushButton::mouseUp   (wxMouseEvent& event)
{
    if( event.LeftUp() )
    {
        m_Value = !m_Value;

        wxCommandEvent my_event(wxEVT_COMMAND_BUTTON_CLICKED, m_ID );
        my_event.SetInt( m_Value );

        // Send it
        GetEventHandler()->ProcessEvent( my_event );

        wxClientDC dc(this);
        this->paintNow(dc);
    }
}
