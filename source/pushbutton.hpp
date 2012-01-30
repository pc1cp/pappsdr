//=============================================================================
//
// file: gui_togglebutton.h
//
// purpose: defining a custom-widget: wxCustomPushButton
//
//=============================================================================

#ifndef __WXCUSTOMPUSHBUTTON_HPP__
#define __WXCUSTOMPUSHBUTTON_HPP__

#include "config.hpp"

extern char pushbutton_png[];
extern char pushbutton_hover_png[];
extern char pushbutton_active_png[];
extern char pushbutton_active_hover_png[];

extern unsigned long pushbutton_png_size;
extern unsigned long pushbutton_hover_png_size;
extern unsigned long pushbutton_active_png_size;
extern unsigned long pushbutton_active_hover_png_size;

DECLARE_EVENT_TYPE(wxEVT_CUSTOM_BUTTON_EVENT, -1)

class wxCustomPushButton: public wxPanel
{
    public:

    // ------------------------------------------------------------------------
    // constructor

    wxCustomPushButton( wxWindow* parent,
                        int ID,
                        wxString label,
                        bool initialstate = false);

    // ------------------------------------------------------------------------
    // virtual destructor, so destructor of parent-class is not ommitted
    // this will avoid hard to find memory-leaks.

    virtual ~wxCustomPushButton();

    // ------------------------------------------------------------------------
    // getters and setters in wxWidget-Style

    inline bool getValue()
    {
        return( m_Value );
    };

    inline void setValue(bool value)
    {
        m_Value = value;
        Refresh();
    };

    // ------------------------------------------------------------------------
    // Painting function for the new widget

    void paintNow(wxDC& dc);

    // ------------------------------------------------------------------------
    // Eventhandling functions

    void mouseDown          (wxMouseEvent& event);
    void mouseDClick        (wxMouseEvent& event);
    void mouseUp            (wxMouseEvent& event);
    void mouseEnter         (wxMouseEvent& event);
    void mouseLeave         (wxMouseEvent& event);
    void paintEvent         (wxPaintEvent& event);
    void onEraseEvent       (wxEraseEvent& event);

    DECLARE_EVENT_TABLE()

    private:

    // ------------------------------------------------------------------------
    // we need some place to store the internal state

    int         m_ID;
    bool        m_Value;
    bool        m_Hover;

    wxString    m_Label;
    wxFont*     m_Font;

    wxSize      m_Size;

    // ------------------------------------------------------------------------
    // we need some graphical data to represent the button on screen

    static int          m_ReferenceCount;
    static wxBitmap*    m_ToggleButtonBitmap1;
    static wxBitmap*    m_ToggleButtonBitmap2;
    static wxBitmap*    m_ToggleButtonBitmap3;
    static wxBitmap*    m_ToggleButtonBitmap4;
    wxMemoryDC          m_MemoryDC;
};

#endif
