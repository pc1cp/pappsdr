#ifndef __WXCUSTOMSPINCTRL_HPP__
#define __WXCUSTOMSPINCTRL_HPP__

#include "wx/wx.h"
#include "wx/spinbutt.h"

extern const wxEventType wxEVT_CUSTOM_SPINCTRL;

class wxCustomSpinCtrl : public wxPanel
{
    public:

    wxCustomSpinCtrl( wxWindow*         parent,
                      wxWindowID        id        =      -1,
                      float             min       = -100.0f,
                      float             max       = +100.0f,
                      float             initial   =    0.0f,
                      float             increment =    1.0f,
                      int               digits    =       2,
                      const wxString&   name      = _T("wxSpinCtrl") );

   ~wxCustomSpinCtrl();

    void  setValue  ( float value );
    float getValue  ();

    private:
    void printValue ( float value );

    void onSpinDown ( wxSpinEvent& event );
    void onSpinUp   ( wxSpinEvent& event );
    void onTextEnter( wxCommandEvent& event );

    wxSpinButton*   m_SpinButton;
    wxTextCtrl*     m_TextControl;

    wxWindow*       m_Parent;
    wxWindowID      m_ID;

    float           m_Value;
    float           m_MaxValue;
    float           m_MinValue;
    float           m_Increment;
    int             m_Digits;

    DECLARE_EVENT_TABLE();
};

#endif
