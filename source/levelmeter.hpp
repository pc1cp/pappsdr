#include "wx/wx.h"

extern unsigned char levelmeter_png[];
extern unsigned char levelmeter_active_png[];

extern unsigned long levelmeter_png_size;
extern unsigned long levelmeter_active_png_size;

class wxCustomLevelMeter : public wxPanel
{
    public:
    wxCustomLevelMeter(wxWindow* parent);
    virtual ~wxCustomLevelMeter();

    private:

    void onTimer        ( wxTimerEvent& event );
    void onPaint        ( wxPaintEvent& event );
	void onErase	    ( wxEraseEvent& event );

    void render         ( wxDC& dc );

    wxWindow*           m_Parent;
    wxSize              m_Size;
    wxMemoryDC          m_MemoryDC;
    wxTimer*            m_Timer;

    int                 m_TopLineInput;
    int                 m_TopLineOutput;

    static int          m_ReferenceCount;
    static wxBitmap*    m_LevelBitmap1;
    static wxBitmap*    m_LevelBitmap2;

    DECLARE_EVENT_TABLE();
};
