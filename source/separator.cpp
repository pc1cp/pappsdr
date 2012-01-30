//=============================================================================
//
// file:
//
// purpose: implementing a custom-widget-pushbutton
//
//=============================================================================

#include "separator.hpp"
#include "wx/mstream.h"

BEGIN_EVENT_TABLE(wxCustomSeparator, wxPanel)

    // catch paint events
    EVT_PAINT               (wxCustomSeparator::paintEvent  )
    EVT_ERASE_BACKGROUND    (wxCustomSeparator::onEraseEvent)

END_EVENT_TABLE()


// ============================================================================
// define static class variables
// ============================================================================

int          wxCustomSeparator::m_ReferenceCount       = 0;
wxBitmap*    wxCustomSeparator::m_SeparatorBitmap1     = 0;

//=============================================================================
// constructor
//=============================================================================

wxCustomSeparator::wxCustomSeparator( wxWindow* parent,
                                      wxString  label,
                                      int width )
                                      :
                                      wxPanel(parent),
                                      m_Label(label)
{
    // if not existing, create bitmap-pointer...
    if( !m_SeparatorBitmap1 )
    {
        wxMemoryInputStream image_data(separator_png, separator_png_size );
        m_SeparatorBitmap1 = new wxBitmap( wxImage( image_data ) );
    }

    // we use reference-counting for the bitmap-pointers...
    m_ReferenceCount++;

    m_Size = wxSize( width,
                     m_SeparatorBitmap1->GetHeight() );
    SetMinSize( m_Size );
    SetMaxSize( m_Size );

    // aquire font of appropriate size
    m_Font = wxFont::New( wxSize( 0, 18 ), wxFONTFAMILY_SWISS );
}


//=============================================================================
// destructor
//=============================================================================

wxCustomSeparator::~wxCustomSeparator()
{
    m_ReferenceCount--;

    if( m_ReferenceCount == 0 )
    {
        delete m_SeparatorBitmap1;
    }
}

//=============================================================================
// Paint the whole thing on screen
//=============================================================================

void wxCustomSeparator::paintNow(wxDC& dc)
{
    // select background bitmap into memory-DC
    m_MemoryDC.SelectObject(*m_SeparatorBitmap1);

    // blit it on screen
    dc.DestroyClippingRegion();
    dc.Blit( 0, 0, m_Size.x, m_Size.y, &m_MemoryDC, 0, 0, wxCOPY, false);

    wxCoord tW;
    wxCoord tH;
    dc.SetFont( *m_Font );
    dc.GetTextExtent( m_Label, &tW, &tH );

    dc.SetTextForeground( *wxLIGHT_GREY );
    dc.DrawText( m_Label, 16 , (m_Size.y - tH)/2 );

    // free memory-DC after painting (required for Win32)
    m_MemoryDC.SelectObject(wxNullBitmap);
}

//=============================================================================
// Eventhandling
//=============================================================================

void wxCustomSeparator::onEraseEvent(wxEraseEvent& WXUNUSED(event))
{
    // do nothing to avoid flicker on Win32...
}

void wxCustomSeparator::paintEvent(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    this->paintNow(dc);
}
