//==============================================================================
//
// Decoder-Software for the Pappradio V 2.0
//
// Copyright (C) 2011/2012  Stefan Fendt
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the  GNU Lesser General Public License  as published by the Free
// Software Foundation;  either version 2.1 of the License,  or (at your option)
// any later version.
//
// This library is distributed  in the hope that it will be useful,  but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You  should have  received  a copy  of the  GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// =============================================================================

// =============================================================================
// include what we need ...
// =============================================================================

#include "wx/wx.h"
#include "kiss_fft.h"

// =============================================================================
// Forward-declare what we do not need *here*, but later...
// =============================================================================

class ComplexSample;

// =============================================================================
// We can send events, so we define them here as extern ...
// =============================================================================

extern const wxEventType wxFFTClicked;

// =============================================================================
// Declaration of the FFT-Display-Widget
// =============================================================================

class wxCustomFFTDisplay : public wxPanel
{
    public:

    enum FFTMODE
    {
        FFT_RADIOFREQ,
        FFT_AUDIOFREQ
    };

    enum FFTDISPLAY
    {
        FFT_WATERFALL,
        FFT_SPECTRUM
    };

    wxCustomFFTDisplay( wxWindow* parent, enum FFTMODE mode );
   ~wxCustomFFTDisplay();

    void setTune    ( double frequency );
    void setLO      ( double frequency );

    void onPaint    ( wxPaintEvent& event );
    void onErase    ( wxEraseEvent& event );

    void paintNow   ();
    void render     ( wxDC& dc );

    void onTimer    ( wxTimerEvent& event );
    void onClick    ( wxMouseEvent& event );

    private:

    void swapBuffers();
    void drawWaterfall();
    void updateWaterfall();
    void drawScale();

    DECLARE_EVENT_TABLE();

    wxWindow*       m_Parent;

    enum FFTMODE    m_Mode;
    enum FFTDISPLAY m_DisplayMode;

    wxTimer*        m_Timer;

    int             m_Width;
    int             m_Height;

    wxMemoryDC      m_SourceDC;
    wxMemoryDC      m_DestinDC;

    wxBitmap*       m_FrontBuffer;
    wxBitmap*       m_BackBuffer;

    wxImage*        m_WaterfallImage;
    unsigned char*  m_WaterfallData;
    int             m_WaterfallLine;

    wxFont*         m_LabelFont;

    int             m_TuneMarker0;
    int             m_TuneMarker1;
    int             m_TuneMarker2;

    float           m_MinFrequency;
    float           m_MaxFrequency;
    float           m_TuneFrequency;

    int             m_FFTSize;
    kiss_fft_cpx*   m_iBuffer;
    kiss_fft_cpx*   m_oBuffer0;
    kiss_fft_cpx*   m_oBuffer1;

    float*          m_WindowBuffer;
    float           m_WindowCorrection;

    float           m_aBuffer[1024];
    float           m_hBuffer[1024];

    ComplexSample*  m_RingBuffer;
    int             m_RingBufferPointer;
    int             m_RingBufferSize;

    kiss_fft_cfg    m_KissCFG;
};

