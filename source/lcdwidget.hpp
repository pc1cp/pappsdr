#include "wx/wx.h"

extern const wxEventType wxLOFreqChanged;
extern const wxEventType wxFreqChanged;

extern unsigned char LCD_background_png[];
extern unsigned char LCD_backgrounddB_png[];
extern unsigned char LCD_foreground_png[];
extern unsigned char LCD_largenumbers_png[];
extern unsigned char LCD_smallnumbers_png[];

extern unsigned long LCD_background_png_size;
extern unsigned long LCD_backgrounddB_png_size;
extern unsigned long LCD_foreground_png_size;
extern unsigned long LCD_largenumbers_png_size;
extern unsigned long LCD_smallnumbers_png_size;

class wxCustomLCDisplay : public wxPanel
{
    public:
    wxCustomLCDisplay(wxWindow* parent);
    virtual ~wxCustomLCDisplay();

    void onPaint    ( wxPaintEvent& event );
	void onErase	( wxEraseEvent& event );

    void paintNow   ();
    void render     ( wxDC& dc );

    void setLOFreq  ( int frequency );
    int  getLOFreq  ();
    void setLOFreqDisplay  ( int frequency );
    int  getLOFreqDisplay  ();

    void setFreq  ( int frequency );
    int  getFreq  ();
    void setFreqDisplay  ( int frequency );
    int  getFreqDisplay  ();

    void setAttenuatorValue ( float valuedB ){ m_AttenuatorValue=valuedB;}

    private:

    void OnMouseWheel( wxMouseEvent& event );
    void onTimer     ( wxTimerEvent& event );
    void onClicked   ( wxMouseEvent& event );
    void onEnter     ( wxMouseEvent& event );
    void onLeave     ( wxMouseEvent& event );

    void renderDigit ( wxDC& dc, wxCoord x, wxCoord y, int number, bool size );
    void renderNumber( wxDC& dc, wxCoord x, wxCoord y, int number );

    DECLARE_EVENT_TABLE();

    bool            m_HasFocus;

    wxSize          m_Size;
    wxWindow*       m_Parent;
    wxFont*         m_Font;

    wxSize          m_CharSize;
    int             m_Width;
    int             m_Height;

    bool            m_dBMode;

    int             m_LOFrequency;
    int             m_LOFrequencyDisplay;
    int             m_Frequency;
    int             m_FrequencyDisplay;

    float           m_SquelchLevel;
    float           m_SignalLevel;
    float           m_SignalLevelPeak;

    static int          m_ReferenceCount;
    wxMemoryDC          m_MemoryDC;
    static wxBitmap*    m_LCDBitmap1;
    static wxBitmap*    m_LCDBitmap2;
    static wxBitmap*    m_LCDBitmap3;
    static wxBitmap*    m_LCDBitmap4;
    static wxBitmap*    m_LCDBitmap5;

    float           m_AttenuatorValue;

    wxTimer*        m_Timer;
};

