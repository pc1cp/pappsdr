#include "levelmeter.hpp"
#include "config.hpp"
#include "wx/mstream.h"
#include "wx/graphics.h"

BEGIN_EVENT_TABLE( wxCustomLevelMeter, wxPanel )
    EVT_TIMER           ( wxID_ANY, wxCustomLevelMeter::onTimer )
    EVT_PAINT           ( wxCustomLevelMeter::onPaint )
    EVT_ERASE_BACKGROUND( wxCustomLevelMeter::onErase  )
END_EVENT_TABLE()

// ============================================================================
// define static class variables
// ============================================================================

int          wxCustomLevelMeter::m_ReferenceCount  = 0;
wxBitmap*    wxCustomLevelMeter::m_LevelBitmap1    = 0;
wxBitmap*    wxCustomLevelMeter::m_LevelBitmap2    = 0;

wxCustomLevelMeter::wxCustomLevelMeter( wxWindow* parent )
    :
    wxPanel ( parent ),
    m_Parent( parent )
{
    // if not existing, create bitmap-pointer...
    if( !m_LevelBitmap1 )
    {
        wxMemoryInputStream image_data(levelmeter_png, levelmeter_png_size);
        m_LevelBitmap1 = new wxBitmap( wxImage( image_data ) );
    }

    if( !m_LevelBitmap2 )
    {
        wxMemoryInputStream image_data(levelmeter_active_png, levelmeter_active_png_size);
        m_LevelBitmap2 = new wxBitmap( wxImage( image_data ) );
    }

    // we use reference-counting for the bitmap-pointers...
    m_ReferenceCount++;

    // our widget is exactly the size of the bitmaps used...
    m_Size = wxSize( m_LevelBitmap1->GetWidth(),
                     m_LevelBitmap1->GetHeight() );

    SetMinSize( m_Size );
    SetMaxSize( m_Size );
    SetBackgroundColour( wxColor( 32,32,32 ) );

    m_Timer = new wxTimer( this );
    m_Timer->Start( 100 );
}

wxCustomLevelMeter::~wxCustomLevelMeter()
{
    m_ReferenceCount--;

    if( m_ReferenceCount == 0 )
    {
        delete m_LevelBitmap1;
        delete m_LevelBitmap2;
    }
}

void wxCustomLevelMeter::onErase( wxEraseEvent& WXUNUSED( event ) )
{
    // do nothing (required for windows to avoid flickering)
    return;
}

void wxCustomLevelMeter::onPaint( wxPaintEvent& WXUNUSED( event ) )
{
    wxPaintDC dc( this );
    render( dc );
}

void wxCustomLevelMeter::onTimer( wxTimerEvent& WXUNUSED( event ) )
{
    GlobalConfig* config = GlobalConfig::getInstance();

    double inputLevel  = (-20.f*log10(config->getInputLevel ()))*9.0/6.0;;
    double outputLevel = (-20.f*log10(config->getOutputLevel()))*9.0/6.0;;
    m_TopLineInput  = (int)( inputLevel/3.0)*3+14;
    m_TopLineOutput = (int)(outputLevel/3.0)*3+14;

    if( m_TopLineInput  > 127 ) m_TopLineInput  = 127;
    if( m_TopLineOutput > 127 ) m_TopLineOutput = 127;

    wxClientDC dc( this );
    render( dc );

    return;
}

void wxCustomLevelMeter::render( wxDC& dc )
{
    dc.DestroyClippingRegion();
    dc.DrawBitmap( *m_LevelBitmap1, 0, 0);

    dc.SetClippingRegion( 0, m_TopLineInput,
                         16, m_Size.GetHeight() - m_TopLineInput );
    dc.DrawBitmap( *m_LevelBitmap2, 0, 0);

    dc.DestroyClippingRegion();

    dc.SetClippingRegion( 16, m_TopLineOutput,
                          16, m_Size.GetHeight() - m_TopLineOutput );
    dc.DrawBitmap( *m_LevelBitmap2, 0, 0);
}
