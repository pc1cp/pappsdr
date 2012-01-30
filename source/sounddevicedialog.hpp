
#ifndef __SND_DLG_HPP__
#define __SND_DLG_HPP__

#include "config.hpp"

enum
{
    SND_OUT_CHOICE,
    SND_IN_CHOICE
};

class MySoundDeviceDialog : public wxDialog
{
    public:

    MySoundDeviceDialog(wxWindow *parent);

    void OnButtonPressed( wxCommandEvent& event);
    void OnDeviceChanged( wxCommandEvent& event);

    DECLARE_EVENT_TABLE()

    private:

    wxChoice* m_OutputChoice;
    wxChoice* m_InputChoice;
};

#endif
