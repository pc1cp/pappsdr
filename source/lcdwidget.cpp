#include "lcdwidget.hpp"
#include "config.hpp"
#include "wx/mstream.h"
#include "wx/graphics.h"

const wxEventType wxLOFreqChanged = wxNewEventType();
const wxEventType wxFreqChanged   = wxNewEventType();

BEGIN_EVENT_TABLE( wxCustomLCDisplay, wxPanel )
    EVT_PAINT       ( wxCustomLCDisplay::onPaint )
    EVT_MOUSEWHEEL  ( wxCustomLCDisplay::OnMouseWheel )
    EVT_TIMER       ( wxID_ANY, wxCustomLCDisplay::onTimer )
    EVT_LEFT_DOWN   ( wxCustomLCDisplay::onClicked )
END_EVENT_TABLE()

// ============================================================================
// define static class variables
// ============================================================================

int          wxCustomLCDisplay::m_ReferenceCount       = 0;
wxBitmap*    wxCustomLCDisplay::m_LCDBitmap1  = 0;
wxBitmap*    wxCustomLCDisplay::m_LCDBitmap2  = 0;
wxBitmap*    wxCustomLCDisplay::m_LCDBitmap3  = 0;
wxBitmap*    wxCustomLCDisplay::m_LCDBitmap4  = 0;
wxBitmap*    wxCustomLCDisplay::m_LCDBitmap5  = 0;

wxCustomLCDisplay::wxCustomLCDisplay( wxWindow* parent )
    :
    wxPanel ( parent ),
    m_Parent( parent )
{
    // if not existing, create bitmap-pointer...
    if( !m_LCDBitmap1 )
    {
        wxMemoryInputStream image_data(LCD_background_png, LCD_background_png_size);
        m_LCDBitmap1 = new wxBitmap( wxImage( image_data ) );
    }

    if( !m_LCDBitmap2 )
    {
        wxMemoryInputStream image_data(LCD_foreground_png, LCD_foreground_png_size);
        m_LCDBitmap2 = new wxBitmap( wxImage( image_data ) );
    }

    if( !m_LCDBitmap3 )
    {
        wxMemoryInputStream image_data(LCD_largenumbers_png, LCD_largenumbers_png_size);
        m_LCDBitmap3 = new wxBitmap(wxImage( image_data ) );
    }

    if( !m_LCDBitmap4 )
    {
        wxMemoryInputStream image_data(LCD_smallnumbers_png, LCD_smallnumbers_png_size);
        m_LCDBitmap4 = new wxBitmap( wxImage( image_data ) );
    }

    if( !m_LCDBitmap5 )
    {
        wxMemoryInputStream image_data(LCD_backgrounddB_png, LCD_backgrounddB_png_size);
        m_LCDBitmap5 = new wxBitmap( wxImage( image_data ) );
    }

    // we use reference-counting for the bitmap-pointers...
    m_ReferenceCount++;

    // our widget is exactly the size of the bitmaps used...
    m_Size = wxSize( m_LCDBitmap1->GetWidth(),
                     m_LCDBitmap1->GetHeight() );
    SetMinSize( m_Size );
    SetMaxSize( m_Size );


    m_Font = wxFont::New( wxSize( 0, 24 ), wxFONTFAMILY_SWISS );

    m_SignalLevel = 0.5f;

#if 1
    wxClientDC dc( this );
    dc.SetFont( *m_Font );

    m_CharSize = dc.GetTextExtent( wxT("0") );
    m_Width    = 512;//m_CharSize.x * 11 + 16;
    m_Height   = 128;//m_CharSize.y *  2 + 16;
    //SetSize   ( wxSize( m_Width, m_Height ) );
    //SetMinSize( wxSize( m_Width, m_Height ) );
    //SetMaxSize( wxSize( m_Width, m_Height ) );
#endif

    m_LOFrequency        = 0;
    m_LOFrequencyDisplay = 0;
    m_Frequency          = 0;
    m_FrequencyDisplay   = 0;

    m_SignalLevel        = 0.f;
    m_SignalLevelPeak    = 0.f;

    m_dBMode             = false;

    m_Timer = new wxTimer(this, wxID_ANY);
    m_Timer->Start( 1500 );
}

//=============================================================================
// destructor
//=============================================================================

wxCustomLCDisplay::~wxCustomLCDisplay()
{
    m_ReferenceCount--;

    if( m_ReferenceCount == 0 )
    {
        delete m_LCDBitmap1;
        delete m_LCDBitmap2;
        delete m_LCDBitmap3;
        delete m_LCDBitmap4;
        delete m_LCDBitmap5;
    }
}


void wxCustomLCDisplay::onPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc( this );
    render( dc );
}

void wxCustomLCDisplay::paintNow()
{
    wxClientDC dc( this );
    render( dc );
}

void wxCustomLCDisplay::renderDigit( wxDC& dc,
                                     wxCoord x,
                                     wxCoord y,
                                     int number,
                                     bool size )
{
    if( size == false )
    {
        // large number
        wxCoord xOffset = (number%12)*-32;

        dc.SetClippingRegion( x, y, 32, 64 );
        dc.DrawBitmap( *m_LCDBitmap3, x+xOffset, y, true );
        dc.DestroyClippingRegion();
    }
    else
    {
        // small number
        wxCoord xOffset = (number%10)*-32;

        dc.SetClippingRegion( x, y, 32, 64 );
        dc.DrawBitmap( *m_LCDBitmap4, x+xOffset, y, true );
        dc.DestroyClippingRegion();
    }
}

void wxCustomLCDisplay::renderNumber( wxDC& dc,
                                      wxCoord x,
                                      wxCoord y,
                                      int number )
{
    int digit0 = ( number /         1 ) % 10;
    int digit1 = ( number /        10 ) % 10;
    int digit2 = ( number /       100 ) % 10;
    int digit3 = ( number /      1000 ) % 10;
    int digit4 = ( number /     10000 ) % 10;
    int digit5 = ( number /    100000 ) % 10;
    int digit6 = ( number /   1000000 ) % 10;
    int digit7 = ( number /  10000000 ) % 10;
    int digit8 = ( number / 100000000 ) % 10;

    // draw first digit if needed
    if( number > 99999999 )
    {
        renderDigit( dc, x, y, digit8, false );
    }
    else
    {
        renderDigit( dc, x, y, 11, false );
    }

    // draw second digit if needed
    if( number > 9999999 )
    {
        renderDigit( dc, x+24, y, digit7, false );
    }
    else
    {
        renderDigit( dc, x+24, y, 11, false );
    }

    // draw third digit and dot if needed
    if( number > 999999 )
    {
        renderDigit( dc, x+48, y, digit6, false );
        renderDigit( dc, x+24*3, y,     10, false );
    }
    else
    {
        renderDigit( dc, x+48, y, 11, false );
    }

    if( number > 99999 )
    {
        renderDigit( dc, x+24*3.35, y, digit5, false );
    }
    else
    {
        renderDigit( dc, x+24*3.35, y, 11, false );
    }

    if( number > 9999 )
    {
        renderDigit( dc, x+24*4.35, y, digit4, false );
    }
    else
    {
        renderDigit( dc, x+24*4.35, y, 11, false );
    }

    renderDigit( dc, x+24*5.35, y, digit3, false );
    renderDigit( dc, x+24*6.35, y,     10, false );
    renderDigit( dc, x+24*6.7, y, digit2, true );
    renderDigit( dc, x+24*7.7-4, y, digit1, true );
    renderDigit( dc, x+24*8.7-5, y, digit0, true );
}

void wxCustomLCDisplay::render( wxDC& dc )
{
    dc.DestroyClippingRegion();

    if( m_dBMode == false )
        m_MemoryDC.SelectObject(*m_LCDBitmap1);
    else
        m_MemoryDC.SelectObject(*m_LCDBitmap5);

    dc.Blit( 0, 0, m_Size.x, m_Size.y, &m_MemoryDC, 0, 0, wxCOPY, false);

    // Blit upper region to active-background for squelch-level
    wxCoord lengthSQ = (wxCoord)(round(482.f/3.f*m_SquelchLevel)*3.f);
    m_MemoryDC.SelectObject(*m_LCDBitmap2);
    dc.Blit( 15, 12, lengthSQ, 6, &m_MemoryDC, 15, 12, wxCOPY, false);

    // Blit upper region to active-background for signal-level
    wxCoord lengthSI = (wxCoord)(round(482.f/3.f*m_SignalLevel)*3.f);
    dc.Blit( 15, 20, lengthSI, 10, &m_MemoryDC, 15, 20, wxCOPY, false);

    // Blit upper region to active-background for signal-level
    wxCoord lengthSP = (wxCoord)(round(482.f/3.f*m_SignalLevelPeak)*3.f);
    dc.Blit( 15+lengthSP, 20, 2, 10, &m_MemoryDC, 15, 20, wxCOPY, false);

    // free memory-DC after painting (required for Win32)
    m_MemoryDC.SelectObject(wxNullBitmap);

    renderNumber( dc,  16, 48, m_LOFrequencyDisplay );
    renderNumber( dc, 272, 48, m_FrequencyDisplay );
}

void wxCustomLCDisplay::OnMouseWheel( wxMouseEvent& event )
{
    wxPoint pos = event.GetPosition();

    if( pos.x > ( 20    ) && pos.y > ( 68    ) &&
        pos.x < ( 20+ 24) && pos.y < ( 68+ 42) )
    {
        // first digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 100000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 100000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > ( 44    ) && pos.y > ( 68    ) &&
        pos.x < ( 44+ 24) && pos.y < ( 68+ 42) )
    {
        // 2nd digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 10000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 10000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > ( 68    ) && pos.y > ( 68    ) &&
        pos.x < ( 68+ 24) && pos.y < ( 68+ 42) )
    {
        // 3rd digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 1000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 1000000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (102    ) && pos.y > ( 68    ) &&
        pos.x < (102+ 24) && pos.y < ( 68+ 42) )
    {
        // 4th digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 100000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 100000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (124    ) && pos.y > ( 68    ) &&
        pos.x < (124+ 24) && pos.y < ( 68+ 42) )
    {
        // 5th digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 10000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 10000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (148    ) && pos.y > ( 68    ) &&
        pos.x < (148+ 24) && pos.y < ( 68+ 42) )
    {
        // 6th digit, LO
        if( event.GetWheelRotation() >= 0 )
        {
            m_LOFrequency += 1000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_LOFrequency -= 1000;
            wxCommandEvent _Event( wxLOFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (380    ) && pos.y > ( 68    ) &&
        pos.x < (380+ 24) && pos.y < ( 68+ 42) )
    {
        // 5th digit, Tune
        if( event.GetWheelRotation() >= 0 )
        {
            m_Frequency += 10000;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_Frequency -= 10000;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (404    ) && pos.y > ( 68    ) &&
        pos.x < (404+ 24) && pos.y < ( 68+ 42) )
    {
        // 5th digit, Tune
        if( event.GetWheelRotation() >= 0 )
        {
            m_Frequency += 1000;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_Frequency -= 1000;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (434    ) && pos.y > ( 68    ) &&
        pos.x < (434+ 20) && pos.y < ( 68+ 42) )
    {
        // 5th digit, Tune
        if( event.GetWheelRotation() >= 0 )
        {
            m_Frequency += 100;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_Frequency -= 100;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (454    ) && pos.y > ( 68    ) &&
        pos.x < (454+ 20) && pos.y < ( 68+ 42) )
    {
        // 5th digit, Tune
        if( event.GetWheelRotation() >= 0 )
        {
            m_Frequency += 10;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_Frequency -= 10;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    if( pos.x > (474    ) && pos.y > ( 68    ) &&
        pos.x < (474+ 20) && pos.y < ( 68+ 42) )
    {
        // 5th digit, Tune
        if( event.GetWheelRotation() >= 0 )
        {
            m_Frequency += 1;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
        else
        {
            m_Frequency -= 1;
            wxCommandEvent _Event( wxFreqChanged );
            wxPostEvent( m_Parent, _Event);
        }
    }

    GlobalConfig* config = GlobalConfig::getInstance();
    double sampleRate = config->getInputSampleRate();

    m_LOFrequency = m_LOFrequency <         0 ?         0:m_LOFrequency;
    m_LOFrequency = m_LOFrequency > 999999999 ? 999999999:m_LOFrequency;

    // limit tuning against current input-samplerate...
    m_Frequency   = m_Frequency   < -sampleRate/2.0 ? -sampleRate/2.0:m_Frequency;
    m_Frequency   = m_Frequency   > +sampleRate/2.0 ? +sampleRate/2.0:m_Frequency;

    // do not allow negative Frequencies as tuning-range...
    if( m_Frequency < -m_LOFrequencyDisplay )
    {
        m_Frequency = -m_LOFrequencyDisplay;
    }
}

void wxCustomLCDisplay::onTimer( wxTimerEvent& WXUNUSED(event) )
{
    GlobalConfig* config = GlobalConfig::getInstance();

    static int cnt = 0;
    cnt = (cnt+1)%30;

    // get raw maximum signal-level since last call
    float signalLevel  = config->getSignalLevel ();
    float squelchLevel = config->getSquelchLevel();

    // convert to dB
    signalLevel  = 20.f*log10(signalLevel);
    squelchLevel = 20.f*log10(squelchLevel);

    signalLevel  += config->getSLevelCorrection(); // calibration-offset
    squelchLevel += config->getSLevelCorrection();

    // map signal-level to 0...1
    signalLevel = (90.f+signalLevel)/90.f;
    signalLevel = (signalLevel<0.f) ? 0.f:signalLevel;
    signalLevel = (signalLevel>1.f) ? 1.f:signalLevel;

    // store it
    m_SignalLevel = signalLevel;

    // map squelch-level to 0...1
    squelchLevel = (90.f+squelchLevel)/90.f;
    squelchLevel = (squelchLevel<0.f) ? 0.f:squelchLevel;
    squelchLevel = (squelchLevel>1.f) ? 1.f:squelchLevel;

    // store it
    m_SquelchLevel = squelchLevel;

    if( cnt != 0 )
    {
        m_SignalLevelPeak = m_SignalLevelPeak < m_SignalLevel?
                            m_SignalLevel:
                            m_SignalLevelPeak;
    }
    else
    {
        m_SignalLevelPeak = m_SignalLevel;
    }

    Refresh();
}

void wxCustomLCDisplay::setLOFreq( int frequency )
{
    m_LOFrequency = frequency;
    m_LOFrequency = m_LOFrequency <         0 ?         0:m_LOFrequency;
    m_LOFrequency = m_LOFrequency > 999999999 ? 999999999:m_LOFrequency;

    // do not allow negative Frequencies as tuning-range...
    if( m_Frequency < -m_LOFrequencyDisplay )
    {
        m_Frequency = -m_LOFrequencyDisplay;
    }
}

int wxCustomLCDisplay::getLOFreq()
{
    return( m_LOFrequency );
}

void wxCustomLCDisplay::setLOFreqDisplay( int frequency )
{
    m_LOFrequencyDisplay = frequency;
    Refresh();
}

int wxCustomLCDisplay::getLOFreqDisplay()
{
    return( m_LOFrequencyDisplay );
}

void wxCustomLCDisplay::setFreq( int frequency )
{
    m_Frequency = frequency;

    // limit tuning against current input-samplerate...
    GlobalConfig* config = GlobalConfig::getInstance();
    double sampleRate = config->getInputSampleRate();

    m_Frequency   = m_Frequency   < -sampleRate/2.0 ? -sampleRate/2.0:m_Frequency;
    m_Frequency   = m_Frequency   > +sampleRate/2.0 ? +sampleRate/2.0:m_Frequency;

    // do not allow negative Frequencies as tuning-range...
    if( m_Frequency < -m_LOFrequencyDisplay )
    {
        m_Frequency = -m_LOFrequencyDisplay;
    }
}

int wxCustomLCDisplay::getFreq()
{
    return( m_Frequency );
}

void wxCustomLCDisplay::setFreqDisplay( int frequency )
{
    m_FrequencyDisplay = frequency;
    Refresh();
}

int wxCustomLCDisplay::getFreqDisplay()
{
    return( m_FrequencyDisplay );
}

void wxCustomLCDisplay::onClicked(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    if( pos.y >= 32 && pos.y <= 44 )
    {
        // flip dB-Mode
        m_dBMode = !m_dBMode;
    }
    else
    if( pos.x >=  15 &&
        pos.y >=   8 &&
        pos.x <= 496 &&
        pos.y <=  20 )
    {
        // calculate absolute squelch-level from relative
        // display-value (clicked position)
        float squelchLevel = (float)(pos.x-15)/482.f;

        // convert into dB according to displayed 90 dB range
        squelchLevel = squelchLevel*90.f-90.f;

        // compensate for calibration-offset
        squelchLevel -= GlobalConfig::getInstance()->getSLevelCorrection();

        // turn dB into absolute audio-/soundcard-level
        squelchLevel = powf( 10.f, squelchLevel/20.f );

        // set it
        GlobalConfig::getInstance()->setSquelchLevel( squelchLevel );
    }

}
