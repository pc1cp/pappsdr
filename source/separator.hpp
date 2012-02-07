//=============================================================================
//
// file: separator.hpp
//
// purpose: defining a custom-widget: wxCustomSeparator
//
//=============================================================================

#ifndef __WXCUSTOMSEPARATOR_HPP__
#define __WXCUSTOMSEPARATOR_HPP__

#include "config.hpp"

extern unsigned char separator_png[];

extern unsigned long separator_png_size;

class wxCustomSeparator: public wxPanel
{
    public:

    // ------------------------------------------------------------------------
    // constructor

    wxCustomSeparator( wxWindow* parent,
                       wxString label,
                       int width = 512 );

    // ------------------------------------------------------------------------
    // virtual destructor, so destructor of parent-class is not ommitted
    // this will avoid hard to find memory-leaks.

    virtual ~wxCustomSeparator();

    // ------------------------------------------------------------------------
    // Painting function for the new widget

    void paintNow(wxDC& dc);

    // ------------------------------------------------------------------------
    // Eventhandling functions

    void paintEvent         (wxPaintEvent& event);
    void onEraseEvent       (wxEraseEvent& event);

    DECLARE_EVENT_TABLE()

    private:

    // ------------------------------------------------------------------------
    // we need some place to store the internal state

    wxString    m_Label;
    wxFont*     m_Font;

    wxSize      m_Size;

    // ------------------------------------------------------------------------
    // we need some graphical data to represent the button on screen

    static int          m_ReferenceCount;
    static wxBitmap*    m_SeparatorBitmap1;
    wxMemoryDC          m_MemoryDC;
};

#endif
