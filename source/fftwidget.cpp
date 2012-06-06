#include "fftwidget.hpp"
#include "config.hpp"

const wxEventType wxFFTClicked = wxNewEventType();

enum
{
    ID_TIMER
};

BEGIN_EVENT_TABLE( wxCustomFFTDisplay, wxPanel )

    EVT_PAINT               (           wxCustomFFTDisplay::onPaint )
    EVT_ERASE_BACKGROUND    (           wxCustomFFTDisplay::onErase )
    EVT_TIMER               ( ID_TIMER, wxCustomFFTDisplay::onTimer )
    EVT_LEFT_DOWN           (           wxCustomFFTDisplay::onClick )

END_EVENT_TABLE()

wxCustomFFTDisplay::wxCustomFFTDisplay( wxWindow* parent, enum FFTMODE mode )
    :
    wxPanel             (        parent ),
    m_Parent            (        parent ),
    m_Mode              (        mode   ),
    m_DisplayMode       ( FFT_WATERFALL ),
    m_FrontBuffer       (             0 ),
    m_BackBuffer        (             0 ),
    m_WaterfallImage    (             0 ),
    m_WaterfallData     (             0 ),
    m_WaterfallLine     (             0 ),
    m_LabelFont         (             0 ),
    m_TuneMarker0       (             0 ),
    m_TuneMarker1       (             0 ),
    m_TuneMarker2       (             0 ),
    m_MinFrequency      (             0 ),
    m_MaxFrequency      (             0 ),
    m_FFTSize           (          2048 ),
    m_iBuffer           (             0 ),
    m_oBuffer0          (             0 ),
    m_oBuffer1          (             0 ),
    m_RingBuffer        (             0 ),
    m_RingBufferPointer (             0 ),
    m_RingBufferSize    (             0 )
{
    // ========================================================================
    // Depending on the mode (audio or RF analysis) setup the widget
    // ========================================================================

    if( m_Mode == FFT_RADIOFREQ )
    {
        // Setup widget size ---------------------------------------------------
        m_Width    = 1024;
        m_Height   = 160;
        m_FFTSize  = 2048;
    }
    else
    {
        // Setup widget size ---------------------------------------------------
        m_Width    = 512;
        m_Height   = 128;
        m_FFTSize  = 2048;
    }

    // get front- and back-buffer-images of that size --------------------------
    m_FrontBuffer = new wxBitmap( m_Width, m_Height, -1 );
    m_BackBuffer  = new wxBitmap( m_Width, m_Height, -1 );

    // fill front and backbuffer with different colours (for debugging)
    {
        wxMemoryDC memDC;
        memDC.SelectObject( *m_FrontBuffer );
        memDC.SetPen( *wxBLACK_PEN );
        memDC.SetBrush( *wxBLACK_BRUSH );
        memDC.DrawRectangle( 0,0, m_Width, m_Height );
        memDC.SelectObject( *m_BackBuffer );
        memDC.SetPen( *wxWHITE_PEN );
        memDC.SetBrush( *wxWHITE_BRUSH );
        memDC.DrawRectangle( 0,0, m_Width, m_Height );
    }

    // get a wxImage for the waterfall-lines -----------------------------------
    m_WaterfallImage = new wxImage( m_Width, m_Height );
    m_WaterfallData  = m_WaterfallImage->GetData();
    m_WaterfallLine  = 0;

    // initialize the waterfall-image with a dark blue color -------------------
    for( int n=0; n<m_Width*m_Height; ++n )
    {
        m_WaterfallData[n*3+0] =  0;
        m_WaterfallData[n*3+1] =  0;
        m_WaterfallData[n*3+2] = 64;
    }

    // populate widget-size to layouter ----------------------------------------
    SetSize   ( wxSize( m_Width, m_Height ) );
    SetMinSize( wxSize( m_Width, m_Height ) );
    SetMaxSize( wxSize( m_Width, m_Height ) );

    // Setup Kiss-FFT ----------------------------------------------------------
    m_KissCFG        = kiss_fft_alloc( m_FFTSize, 0, 0, 0 );

    m_iBuffer        = new kiss_fft_cpx  [ m_FFTSize   ];
    m_oBuffer0       = new kiss_fft_cpx  [ m_FFTSize   ];
    m_oBuffer1       = new kiss_fft_cpx  [ m_FFTSize   ];
    m_WindowBuffer   = new float         [ m_FFTSize   ];
    m_RingBuffer     = new ComplexSample [ m_FFTSize*4 ];
    m_RingBufferSize = m_FFTSize*4;

    m_WindowCorrection = 0.f;
    for( int n=0; n<m_FFTSize; ++n )
    {
        // Blackman-Harris-Window
        double const a0 = 0.35875;
        double const a1 = 0.48829;
        double const a2 = 0.14128;
        double const a3 = 0.01168;
        m_WindowBuffer[n] =
                   a0
                  -a1*cos(2.0*M_PI*(double)n/(double)(m_FFTSize-1))
                  +a2*cos(4.0*M_PI*(double)n/(double)(m_FFTSize-1))
                  -a3*cos(6.0*M_PI*(double)n/(double)(m_FFTSize-1));
        m_WindowCorrection += m_WindowBuffer[n];
    }

    // Setup the timer to 15/30 FFT-Lines / second -----------------------------
    m_Timer = new wxTimer( this, ID_TIMER );
    if( m_Mode == FFT_RADIOFREQ )
        m_Timer->Start(1000/15);
    else
        m_Timer->Start(1000/15);

    // Setup Font --------------------------------------------------------------
    m_LabelFont = wxFont::New( wxSize( 0, 12 ), wxFONTFAMILY_SWISS );

    #if 0
    this->SetBackgroundColour( *wxBLACK );


    m_Line = 0;
    m_Image = new wxImage( m_Width, m_Height );
    m_BackBuffer = new wxImage( m_Width, m_Height );

    m_KissCFG = kiss_fft_alloc( 4096, 0, 0, 0 );

    // Calculate TickDistance
    float samplerate = 44100;
    float range      = samplerate/2000.0;
    float ticksHz    = 512.0 / range;
    m_TickDistance = ticksHz;

    std::cerr << "PixelTicks:" << ticksHz << "\n";

    for( int n=0; n<1024; n+=1 )
    {
        m_hBuffer[n] = 0;
        m_aBuffer[n] = 0;
    }

    m_RingBufferPointer = 0;

    m_TuneMarker = 0;
    #endif
}

wxCustomFFTDisplay::~wxCustomFFTDisplay()
{
    m_Timer->Stop();

    m_DestinDC.SelectObject( wxNullBitmap );
    m_SourceDC.SelectObject( wxNullBitmap );

    kiss_fft_free( m_KissCFG );

    delete    m_FrontBuffer;
    delete    m_BackBuffer;
    delete    m_WaterfallImage;
    delete [] m_iBuffer;
    delete [] m_oBuffer0;
    delete [] m_oBuffer1;
    delete [] m_RingBuffer;
    delete [] m_WindowBuffer;
}

void wxCustomFFTDisplay::onErase( wxEraseEvent& WXUNUSED(event) )
{
    // just do nothing (prevents flicker on some architectures)...
}

void wxCustomFFTDisplay::onPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc( this );
    render( dc );
}

void wxCustomFFTDisplay::paintNow()
{
    wxClientDC dc( this );
    render( dc );
}

void wxCustomFFTDisplay::swapBuffers()
{
    wxBitmap*     swap;
    swap          = m_BackBuffer;
    m_BackBuffer  = m_FrontBuffer;
    m_FrontBuffer = swap;
}

void wxCustomFFTDisplay::drawWaterfall()
{
    // create a screen-bitmap from image-data
    wxBitmap waterfall( *m_WaterfallImage );
    m_SourceDC.SelectObject( waterfall );
    m_DestinDC.SelectObject( *m_BackBuffer );

    int y0 = m_WaterfallLine;
    int y1 = m_Height-m_WaterfallLine;

    m_DestinDC.SetClippingRegion( 0, 0, m_Width, m_Height-32 );

    m_DestinDC.Blit( 0,  0, m_Width, y1, &m_SourceDC, 0, y0 );
    m_DestinDC.Blit( 0, y1, m_Width, y0, &m_SourceDC, 0,  0 );

    m_DestinDC.SetPen  ( wxColour( 255,  96,   0) );
    m_DestinDC.DrawLine( m_TuneMarker1+512, 0, m_TuneMarker1+512, m_Height-32 );
    m_DestinDC.SetPen  ( wxColour(   0,   0,   0) );
    m_DestinDC.DrawLine( m_TuneMarker1+513, 0, m_TuneMarker1+513, m_Height-32 );
    m_DestinDC.DrawLine( m_TuneMarker1+511, 0, m_TuneMarker1+511, m_Height-32 );

    m_DestinDC.DestroyClippingRegion();

    // release memoryDCs before going out of scope
    m_DestinDC.SelectObject( wxNullBitmap );
    m_SourceDC.SelectObject( wxNullBitmap );
}

float colourTable[12][3] =
{
    {   0.0,   0.0,   0.0  }, // below S0
    {   0.0,   0.0, 128.0  }, // S0
    {   0.0,   0.0, 255.0  }, // S1
    {  16.0,  64.0, 255.0  }, // S2
    {  32.0, 128.0, 255.0  }, // S3
    { 144.0,  64.0, 192.0  }, // S4
    { 255.0,   0.0, 128.0  }, // S5
    { 255.0,   0.0,   0.0  }, // S6
    { 255.0, 128.0,   0.0  }, // S7
    { 255.0, 255.0,   0.0  }, // S8
    { 255.0, 255.0, 128.0  }, // S9
    { 255.0, 255.0, 255.0  }  // above S9
};

void wxCustomFFTDisplay::updateWaterfall()
{
    float r=0;
    float g=0;
    float b=0;
    float blend;

    // transfer amplitude-buffer into current line of waterfall-image
    int const iMax = (m_Mode == FFT_RADIOFREQ)? 1024:512;
    for( int i=0; i<iMax; ++i )
    {
        float value = m_aBuffer[i];

        value = 20*log10( value );      // convert into dB
        value += 0.f;                   // gain-offset pappradio, needs to be
                                        // stored in config-singleton later...
        if( m_Mode == FFT_RADIOFREQ )
        {
            value = (96+value)/(96);        // normalize to 0..-96dB range for
                                             // display and clamp
        }
        else
        {
            // for audio-out
            value = (52+value)/(24);        // normalize to 0..-48dB range for
                                            // display and clamp
        }

        if( value < 0 )
            value = 0;
        if( value > 1 )
            value = 1;
        
        #if 0
        if( value < 0.20 )
        {
            blend = value/0.20;
            r = blend*  0.f+  0.f*(1.f-blend);
            g = blend*  0.f+  0.f*(1.f-blend);
            b = blend*128.f+ 64.f*(1.f-blend);
        }
        else
        if( value < 0.40 )
        {
            blend = (value-0.20)/0.20;
            r = blend* 48.f+  0.f*(1.f-blend);
            g = blend*  0.f+  0.f*(1.f-blend);
            b = blend*255.f+128.f*(1.f-blend);
        }
        else
        if( value < 0.60 )
        {
            blend = (value-0.40)/0.20;
            r = blend*255.f+ 48.f*(1.f-blend);
            g = blend* 96.f+  0.f*(1.f-blend);
            b = blend*  0.f+255.f*(1.f-blend);
        }
        else
        if( value <= 0.80 )
        {
            blend = (value-0.60)/0.20;
            r = blend*255.f+255.f*(1.f-blend);
            g = blend*192.f+ 96.f*(1.f-blend);
            b = blend*  0.f+  0.f*(1.f-blend);
        }
        else
        if( value <= 1.00 )
        {
            blend = (value-0.80)/0.20;
            r = blend*255.f+255.f*(1.f-blend);
            g = blend*255.f+192.f*(1.f-blend);
            b = blend*255.f+  0.f*(1.f-blend);
        }
        else
        {
            r = 255.f;
            g = 255.f;
            b = 255.f;
        }
        #else
        int   loIndex = (int)floorf( value * 12.f );
        int   hiIndex = loIndex + 1;
        float a0      = ( value * 12.f ) - (float)loIndex;
        float a1      = 1.f - a0;

        if( hiIndex > 11 )
        {
            hiIndex = 11;
        }

        r = colourTable[loIndex][0] * a1 + colourTable[hiIndex][0] * a0;
        g = colourTable[loIndex][1] * a1 + colourTable[hiIndex][1] * a0;
        b = colourTable[loIndex][2] * a1 + colourTable[hiIndex][2] * a0;
        #endif

        r = (r>255) ? 255:r;
        g = (g>255) ? 255:g;
        b = (b>255) ? 255:b;

        int const _size_ = m_Width*m_Height*3;
        int const _line_ = (m_WaterfallLine+m_Height-33)%m_Height;
        int const _aR_   = ((i+_line_*m_Width)*3+0)%_size_;
        int const _aG_   = ((i+_line_*m_Width)*3+1)%_size_;
        int const _aB_   = ((i+_line_*m_Width)*3+2)%_size_;

        m_WaterfallData[_aR_]=r;
        m_WaterfallData[_aG_]=g;
        m_WaterfallData[_aB_]=b;
    }
}
#include <iostream>
void wxCustomFFTDisplay::drawScale()
{
    GlobalConfig* config = GlobalConfig::getInstance();

    double sampleRate = 0;
    if( m_Mode == FFT_RADIOFREQ)
    {
        sampleRate = config->getInputSampleRate();
    }
    else
    {
        sampleRate = 12000;//config->getProcessingSampleRate();
    }

    m_MinFrequency = m_TuneFrequency - sampleRate/2.f;
    m_MaxFrequency = m_TuneFrequency + sampleRate/2.f;

    float delta = (m_MaxFrequency-m_MinFrequency)/1024.f;

    m_DestinDC.SelectObject( *m_BackBuffer );
    m_DestinDC.SetClippingRegion( 0, m_Height-32, m_Width, m_Height );

    // draw background in dark gray
    m_DestinDC.SetPen  ( wxColour( 32, 32, 32 ) );
    m_DestinDC.SetBrush( wxColour( 32, 32, 32 ) );
    m_DestinDC.DrawRectangle( 0, m_Height-32, m_Width, 32 );

    // draw receiption-range in orange
    m_DestinDC.SetPen  ( wxColour( 255, 96, 0 ) );
    m_DestinDC.SetBrush( wxColour( 255, 96, 0 ) );
    m_TuneMarker2 =  +1;
    m_TuneMarker0 = -15;
    m_DestinDC.DrawRectangle( (m_TuneMarker0+m_TuneMarker1)+512, m_Height-32,
                              (m_TuneMarker2-m_TuneMarker0), 8 );

    m_DestinDC.SetPen  ( wxColour( 128, 128, 128 ) );
    m_DestinDC.SetTextForeground( wxColour( 128, 128, 128 ) );
    m_DestinDC.SetFont( *m_LabelFont );

    if( m_Mode == FFT_RADIOFREQ )
    {
        for( int x=-100; x<1024+100; ++x )
        {
            float f = m_MinFrequency + delta*(float)x;
            if( fabs(floor(0.5+f/10000.f) - f/10000.f)*10000.f < delta/2.f )
            {
                wxString label;
                label.Printf( _("%3.3f"), floor(0.5+f/1000.f)/1000.f );
                wxCoord extX, extY;
                m_DestinDC.GetTextExtent( label, &extX, &extY );
                m_DestinDC.DrawText( label, x-extX/2, m_Height-32+16 );

                m_DestinDC.SetPen  ( wxColour( 128, 128, 128 ) );
                m_DestinDC.DrawLine( x  , m_Height-32, x  , m_Height-32+14 );
                m_DestinDC.SetPen  ( wxColour(   0,   0,   0 ) );
                m_DestinDC.DrawLine( x+1, m_Height-32, x+1, m_Height-32+14 );
            }
            else
            if( fabs(floor(0.5+f/5000.f) - f/5000.f)*5000.f < delta/2.f )
            {
                m_DestinDC.SetPen  ( wxColour( 128, 128, 128 ) );
                m_DestinDC.DrawLine( x  , m_Height-32, x  , m_Height-32+8 );
                m_DestinDC.SetPen  ( wxColour(   0,   0,   0 ) );
                m_DestinDC.DrawLine( x+1, m_Height-32, x+1, m_Height-32+8 );
            }
            else
            if( fabs(floor(0.5+f/1000.f) - f/1000.f)*1000.f < delta/2.f )
            {
                m_DestinDC.SetPen  ( wxColour( 128, 128, 128 ) );
                m_DestinDC.DrawLine( x , m_Height-32, x , m_Height-32+3 );
                m_DestinDC.SetPen  ( wxColour(   0,   0,   0 ) );
                m_DestinDC.DrawLine( x+1, m_Height-32, x+1, m_Height-32+3 );
            }
        }
    }
    else
    if( m_Mode == FFT_AUDIOFREQ )
    for( int x=-100; x<1024+100; ++x )
    {
        float f = delta*(float)x;
        if( fabs(floor(0.5+f/1000.f) - f/1000.f)*1000.f < delta/2.f )
        {
            wxString label;
            label.Printf( _("%1.1f"), floor(0.5+f/1000.f) );
            wxCoord extX, extY;
            m_DestinDC.GetTextExtent( label, &extX, &extY );
            m_DestinDC.DrawLine( x, m_Height-32, x, m_Height-32+14 );
            m_DestinDC.DrawText( label, x-extX/2, m_Height-32+16 );
        }
        else
        if( fabs(floor(0.5+f/500.f) - f/500.f)*500.f < delta/2.f )
        {
            m_DestinDC.DrawLine( x, m_Height-32, x, m_Height-32+8 );
        }
        else
        if( fabs(floor(0.5+f/100.f) - f/100.f)*100.f < delta/2.f )
        {
            m_DestinDC.DrawLine( x, m_Height-32, x, m_Height-32+3 );
        }
    }

    m_DestinDC.DestroyClippingRegion();
    m_DestinDC.SelectObject( wxNullBitmap );
}

void wxCustomFFTDisplay::render( wxDC& dc )
{
    // blit current front buffer on screen -------------------------------------
    wxMemoryDC  memDC( *m_FrontBuffer );
    dc.Blit( 0, 0, m_Width, m_Height, &memDC, 0, 0 );
    memDC.SelectObject( wxNullBitmap );

    // swap buffers ------------------------------------------------------------
    swapBuffers();

    // if required draw the waterfall-image into backbuffer --------------------
    if( m_DisplayMode == FFT_WATERFALL )
    {
        drawWaterfall();
    }

    // draw frequency into the backbuffer --------------------------------------
    drawScale();

    // draw Border
    dc.SetPen( *wxBLACK_PEN );
    dc.SetBrush( *wxTRANSPARENT_BRUSH );
    dc.DrawRectangle( 0,0, m_Width, m_Height );

    #if 0
    #if 0
//    wxMemoryDC memDC( *m_BackBuffer );
//    wxGCDC gc(this);
    wxGraphicsContext *gc = wxGraphicsContext::Create( this );

    if( gc )
    {
        gc->SetPen( *wxBLACK_PEN );
        gc->SetBrush( *wxBLACK_BRUSH );
        gc->DrawRectangle( 0, 0, m_Width, m_Height );

        gc->SetPen( *wxGREY_PEN );
        gc->SetBrush( *wxGREY_BRUSH );

        wxGraphicsPath path = gc->CreatePath();
        for( int n=1; n<1024; n+=2 )
        {
            double x0 = n;
            double x1 = n-2;
            double y0 = (90.0+20.0*log10(m_aBuffer[n  ]))/90.0*-m_Height+m_Height;
            double y1 = (90.0+20.0*log10(m_aBuffer[n-2]))/90.0*-m_Height+m_Height;
            y0 = y0 >= m_Height-1? m_Height-2:y0;
            y1 = y1 >= m_Height-1? m_Height-2:y1;
            path.MoveToPoint( x0, y0 );
            path.AddLineToPoint( x1, y1 );
//            gc->StrokeLine( x1, y1, x0, y0 );
        }
        path.AddLineToPoint( m_Width, m_Height+2 );
        path.MoveToPoint( m_width, m_Height+2 );
        path.AddLineToPoint(0,m_Height+2);

        gc->DrawPath( path );
    }
    delete gc;
    return;

    //dc.Blit( 0, 0, m_Width, m_Height, &memDC, 0, 0 );

    //memDC.SelectObject( wxNullBitmap );
    #endif
#if 0
    for( int n=0; n<1024; n+=1 )
    {
        float v0 = m_aBuffer[n];
        float v1 = m_hBuffer[n];
        if( v0 > v1 )
        {
            m_hBuffer[n] = m_aBuffer[n];
//            m_cBuffer[n] = 10;
        }
        else
        {
//            if( m_cBuffer[n] <= 0 )
                m_hBuffer[n] *= 0.8;
//            else
//                m_cBuffer[n]--;
        }
    }

    dc.SetBrush( wxColour( 0,0,64 ) );
    dc.SetPen( wxColour( 0,0,64 ) );
    dc.DrawRectangle( 0, 0, m_Width, m_Height );

    dc.SetPen( wxColour( 0,32,64 ) );
    for( int y=(m_Height-m_Height/90.f*3.f); y>0; y-=(m_Height/90.f*6.f) )
        dc.DrawLine( 0, y, m_Width, y );

    dc.SetClippingRegion( 0,0,m_Width,m_Height );
    dc.SetPen( wxColour( 96,64,64 ) );
    for( int n=0; n<1024; n+=1 )
    {
        int x0 = n;
        int x1 = n;
        int y0 = (90.0+20.0*log10(m_hBuffer[n  ]))/90.0*-m_Height+m_Height;
        int y1 = m_Height;

        y0 = y0 >= m_Height? m_Height-1:y0;

        dc.DrawLine( x0, y0, x1, y1 );
    }

    dc.SetPen( wxColour( 255,128,0 ) );
    for( int n=1; n<1024; n+=1 )
    {
        int x0 = n;
        int x1 = n-1;
        int y0 = (90.0+20.0*log10(m_aBuffer[n  ]))/90.0*-m_Height+m_Height;
        int y1 = (90.0+20.0*log10(m_aBuffer[n-1]))/90.0*-m_Height+m_Height;

        y0 = y0 >= m_Height? m_Height-1:y0;
        y1 = y1 >= m_Height? m_Height-1:y1;

        dc.DrawLine( x0, y0, x1, y1 );
    }
    dc.DestroyClippingRegion();

    return;
#endif
    unsigned char* data = m_Image->GetData();

    dc.SetBrush( wxColour( 0,0,64 ) );
    dc.SetPen( wxColour( 0,0,64 ) );
    dc.DrawRectangle( 0, 0, m_Width, m_Height );

    if( m_Mode == FFT_RADIOFREQ )
    {
        for(int i=0; i<1024; i+=1 )
        {
            float value = m_aBuffer[i];

            value = 20*log10( value ); // convert into dB
            value += 0.f; // gain-offset pappradio

            value = (90+value)/(90);

            //value = (float)i/1024.f;

            if( value < 0 )
                value = 0;
            if( value > 1 )
                value = 1;

            float r=0;
            float g=0;
            float b=0;

            if( value < 0.25 )
            {
                float blend = value/0.25;
                r = blend*  0.f+  0.f*(1.f-blend);
                g = blend*128.f+  0.f*(1.f-blend);
                b = blend*255.f+ 64.f*(1.f-blend);
            }
            else
            if( value < 0.50 )
            {
                float blend = (value-0.25)/0.25;
                r = blend*128.f+  0.f*(1.f-blend);
                g = blend* 96.f+128.f*(1.f-blend);
                b = blend*128.f+255.f*(1.f-blend);
            }
            else
            if( value < 0.75 )
            {
                float blend = (value-0.50)/0.25;
                r = blend*255.f+128.f*(1.f-blend);
                g = blend* 96.f+ 96.f*(1.f-blend);
                b = blend*  0.f+128.f*(1.f-blend);
            }
            else
            if( value <= 1.00 )
            {
                float blend = (value-0.75)/0.25;
                r = blend*168.f+255.f*(1.f-blend);
                g = blend*168.f+ 96.f*(1.f-blend);
                b = blend*168.f+  0.f*(1.f-blend);
            }
            else
            {
                r = 168.f;
                g = 168.f;
                b = 168.f;
            }

            r = (r>255) ? 255:r;
            g = (g>255) ? 255:g;
            b = (b>255) ? 255:b;

            *(data+i*3+m_Line*3*m_Width+0)=r;
            *(data+i*3+m_Line*3*m_Width+1)=g;
            *(data+i*3+m_Line*3*m_Width+2)=b;
        }
    }
    else
    {
        for(int i=0; i<512; i+=1 )
        {
            float value = m_aBuffer[512+i];

            value = 20*log10( value ); // convert into dB
            value += 40.f; // gain-offset pappradio

            value = (48+value)/(48);

            //value = (float)i/1024.f;

            if( value < 0 )
                value = 0;
            if( value > 1 )
                value = 1;

            float r=0;
            float g=0;
            float b=0;

            if( value < 0.25 )
            {
                float blend = value/0.25;
                r = blend*  0.f+  0.f*(1.f-blend);
                g = blend*128.f+  0.f*(1.f-blend);
                b = blend*255.f+ 64.f*(1.f-blend);
            }
            else
            if( value < 0.50 )
            {
                float blend = (value-0.25)/0.25;
                r = blend*128.f+  0.f*(1.f-blend);
                g = blend* 96.f+128.f*(1.f-blend);
                b = blend*128.f+255.f*(1.f-blend);
            }
            else
            if( value < 0.75 )
            {
                float blend = (value-0.50)/0.25;
                r = blend*255.f+128.f*(1.f-blend);
                g = blend* 96.f+ 96.f*(1.f-blend);
                b = blend*  0.f+128.f*(1.f-blend);
            }
            else
            if( value <= 1.00 )
            {
                float blend = (value-0.75)/0.25;
                r = blend*168.f+255.f*(1.f-blend);
                g = blend*168.f+ 96.f*(1.f-blend);
                b = blend*168.f+  0.f*(1.f-blend);
            }
            else
            {
                r = 168.f;
                g = 168.f;
                b = 168.f;
            }

            r = (r>255) ? 255:r;
            g = (g>255) ? 255:g;
            b = (b>255) ? 255:b;

            *(data+i*3+m_Line*3*m_Width+0)=r;
            *(data+i*3+m_Line*3*m_Width+1)=g;
            *(data+i*3+m_Line*3*m_Width+2)=b;
        }
    }

    wxMemoryDC sourceDC;
    wxMemoryDC destinDC;

    wxBitmap sBitmap( *m_Image );
    wxBitmap dBitmap( *m_BackBuffer );
    sourceDC.SelectObject( sBitmap );
    destinDC.SelectObject( dBitmap );

    destinDC.SetBrush( wxColour( 0,0,64 ) );
    destinDC.SetPen( wxColour( 0,0,64 ) );
    destinDC.DrawRectangle( 0, 0, m_Width, m_Height );

    destinDC.SetClippingRegion( 0,1,m_Width,m_Height-1 );
    destinDC.Blit( 0, 0, m_Width, m_Height-m_Line, &sourceDC, 0, m_Line );
    destinDC.Blit( 0, m_Height-m_Line, m_Width, m_Line, &sourceDC, 0, 0 );
    destinDC.DestroyClippingRegion();

    destinDC.SetBrush( wxColour( 100, 100, 100, 128 ) );
    destinDC.SetPen  ( wxColour( 100, 100, 100, 128 ) );

    wxImage image = dBitmap.ConvertToImage();
    data = image.GetData();

    dc.Blit( 0, 0, m_Width, m_Height, &destinDC, 0, 0 );

    sourceDC.SelectObject( wxNullBitmap );
    destinDC.SelectObject( wxNullBitmap );
    //delete sourceDC;
    //delete destinDC;

    if( m_Mode == FFT_RADIOFREQ )
    {
        dc.SetPen  ( wxColour( 0,0,0 ) );
        dc.SetBrush( wxColour( 0,0,0 ) );
        dc.DrawRectangle( 0, m_Height, m_Width, 32 );

        dc.SetPen  ( wxColour( 128,64,0 ) );
        dc.SetBrush( wxColour( 128,64,0 ) );
        dc.DrawRectangle( m_TuneMarker+512, m_Height+1, 32, 12 );

        dc.SetPen( *wxWHITE_PEN );
        for( double x = 0; x<512; x+=m_TickDistance )
        {
            dc.DrawLine( x+512, m_Height+1, x+512, m_Height+3 );
            dc.DrawLine( 512-x+1, m_Height+1, 512-x+1, m_Height+3 );
        }
        for( double x = 0; x<512; x+=m_TickDistance*5 )
        {
            dc.DrawLine( x+512, m_Height+2, x+512, m_Height+6 );
            dc.DrawLine( 512-x+1, m_Height+2, 512-x+1, m_Height+6 );
        }
        for( double x = 0; x<512; x+=m_TickDistance*10 )
        {
            dc.DrawLine( x+512, m_Height+2, x+512, m_Height+12 );
            dc.DrawLine( 512-x+1, m_Height+2, 512-x+1, m_Height+12 );
        }
    }

    //delete sourceDC;
    #endif
}
#include <fstream>
void wxCustomFFTDisplay::onTimer( wxTimerEvent& WXUNUSED(event) )
{
    GlobalConfig* config = GlobalConfig::getInstance();
    AudioQueue*   fftQueue = 0;

    // paint last buffer on screen ---------------------------------------------
    paintNow();

    // update one FFT-Line -----------------------------------------------------

    if( m_Mode == FFT_RADIOFREQ )
        fftQueue = config->getFFTQueueRF();
    else
        fftQueue = config->getFFTQueueAF();

    if( fftQueue )
    {
        int nrOfSamples = config->getSampleRate();
        nrOfSamples = config->getSampleRate()/30.0;

        // copy into ringbuffer 
        fftQueue->lock();
        int readCount=0;
        while( fftQueue->getLength() > 1000 && readCount < nrOfSamples )
        {
            m_RingBuffer[m_RingBufferPointer] = fftQueue->pull();
            m_RingBufferPointer = (m_RingBufferPointer+1)%m_RingBufferSize;
            readCount++;
        }
        fftQueue->unlock();

        // copy these into the kiss-fft-structure
        for( int n=0; n<m_FFTSize; ++n )
        {
            int const offset = (n+m_RingBufferPointer)%m_RingBufferSize;
            ComplexSample sample = m_RingBuffer[offset];

            sample *=  m_WindowBuffer[n];

            m_iBuffer[n].r = sample.getI();
            m_iBuffer[n].i = sample.getQ();
        }

        kiss_fft( m_KissCFG, m_iBuffer, m_oBuffer0);

        // copy into ringbuffer 
        fftQueue->lock();
        readCount=0;
        while( fftQueue->getLength() > 1000 && readCount < nrOfSamples )
        {
            m_RingBuffer[m_RingBufferPointer] = fftQueue->pull();
            m_RingBufferPointer = (m_RingBufferPointer+1)%m_RingBufferSize;
            readCount++;
        }
        fftQueue->unlock();

        for( int n=0; n<m_FFTSize; ++n )
        {
            int const offset = (n+m_RingBufferPointer+m_RingBufferSize)%m_RingBufferSize;
            ComplexSample sample = m_RingBuffer[offset];

            sample *=  m_WindowBuffer[n];

            m_iBuffer[n].r = sample.getI();
            m_iBuffer[n].i = sample.getQ();
        }

        kiss_fft( m_KissCFG, m_iBuffer, m_oBuffer1);

        if( m_Mode == FFT_RADIOFREQ )
        {
            for( int n=0; n<1024; ++n )
            {
                int offset0 = (double)n*(double)m_FFTSize/1024.0
                             +(double)m_FFTSize/2.0;
                int offset1 = (double)n*(double)m_FFTSize/1024.0
                             +(double)m_FFTSize/2.0;
                offset0 %= m_FFTSize;
                offset1 %= m_FFTSize;

                m_aBuffer[n] =
                         sqrtf( m_oBuffer0[offset0].r *
                                m_oBuffer0[offset0].r +
                                m_oBuffer0[offset0].i *
                                m_oBuffer0[offset0].i )+
                         sqrtf( m_oBuffer0[offset1].r *
                                m_oBuffer0[offset1].r +
                                m_oBuffer0[offset1].i *
                                m_oBuffer0[offset1].i )+
                         sqrtf( m_oBuffer1[offset0].r *
                                m_oBuffer1[offset0].r +
                                m_oBuffer1[offset0].i *
                                m_oBuffer1[offset0].i )+
                         sqrtf( m_oBuffer1[offset1].r *
                                m_oBuffer1[offset1].r +
                                m_oBuffer1[offset1].i *
                                m_oBuffer1[offset1].i );
                m_aBuffer[n] /= m_WindowCorrection*4.f;
            }
        }
        else
        {
            for( int n=0; n<512; ++n )
            {
                int offset0 = (double)(n+0)*(double)m_FFTSize/2.0/512.0;
                int offset1 = (double)(n+1)*(double)m_FFTSize/2.0/512.0;
                int offset2 = (double)(n-1)*(double)m_FFTSize/2.0/512.0;
                offset0 %= m_FFTSize;
                offset1 %= m_FFTSize;

                m_aBuffer[n] =
                         sqrtf( m_oBuffer0[offset0].r *
                                m_oBuffer0[offset0].r +
                                m_oBuffer0[offset0].i *
                                m_oBuffer0[offset0].i )+
                         sqrtf( m_oBuffer0[offset1].r *
                                m_oBuffer0[offset1].r +
                                m_oBuffer0[offset1].i *
                                m_oBuffer0[offset1].i )*0.5f+
                         sqrtf( m_oBuffer0[offset2].r *
                                m_oBuffer0[offset2].r +
                                m_oBuffer0[offset2].i *
                                m_oBuffer0[offset2].i )*0.5f+
                         sqrtf( m_oBuffer1[offset0].r *
                                m_oBuffer1[offset0].r +
                                m_oBuffer1[offset0].i *
                                m_oBuffer1[offset0].i )+
                         sqrtf( m_oBuffer1[offset1].r *
                                m_oBuffer1[offset1].r +
                                m_oBuffer1[offset1].i *
                                m_oBuffer1[offset1].i )*0.5f+
                         sqrtf( m_oBuffer1[offset2].r *
                                m_oBuffer1[offset2].r +
                                m_oBuffer1[offset2].i *
                                m_oBuffer1[offset2].i )*0.5f;
               m_aBuffer[n] /= m_WindowCorrection*4.f;
            }
        }
    }

    m_WaterfallLine = (m_WaterfallLine+1)%m_Height;
    updateWaterfall();
}

void wxCustomFFTDisplay::onClick( wxMouseEvent& event )
{
    if( m_Mode == FFT_RADIOFREQ )
    {
        wxCommandEvent _Event( wxFFTClicked );
        _Event.SetInt( event.GetPosition().x - m_Width/2 );
        wxPostEvent( m_Parent, _Event);
    }
}

void wxCustomFFTDisplay::setTune( double frequency )
{
    GlobalConfig* config = GlobalConfig::getInstance();
    double sampleRate = config->getInputSampleRate();
    double index = 512.0 / (sampleRate/2.0) * frequency;
    m_TuneMarker1 = index;
}

void wxCustomFFTDisplay::setLO( double frequency )
{
    m_TuneFrequency = frequency;
}

