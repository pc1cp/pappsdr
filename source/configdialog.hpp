
#ifndef __CONFIGDIALOG_HPP__
#define __CONFIGDIALOG_HPP__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "spinctrl.hpp"

class   wxCustomConfigDialog : public wxDialog
{
    public:
    wxCustomConfigDialog( wxWindow* parent = 0);
   ~wxCustomConfigDialog();

    private:
    void            onInputDevice     ( wxCommandEvent& event );
    void            onOutputDevice    ( wxCommandEvent& event );
    void            onSampleRate      ( wxCommandEvent& event );
    void            onSampleRateReal  ( wxSpinEvent&    event );
    void            onSampleRatePPM   ( wxSpinEvent&    event );

    wxPanel*        createAudioIOPage ( wxBookCtrlBase* parent );
    wxPanel*        createHardwarePage( wxBookCtrlBase* parent );

    wxWindow*       m_Parent;

    wxButton*       m_ButtonOK;
    wxButton*       m_ButtonCancel;
    wxButton*       m_ButtonDefault;

    wxPanel*        m_AudioIOPanel;
    wxPanel*        m_HardwarePanel;
    wxPanel*        m_ReceiverPanel;

    wxChoice*       m_SampleRateChoice;

    wxCustomSpinCtrl* m_SampleRateSpin;
    wxCustomSpinCtrl* m_SampleRatePPMSpin;

    wxCustomSpinCtrl* m_XTalPPMSpin;

    wxCustomSpinCtrl* m_Att00Spin;
    wxCustomSpinCtrl* m_Att10Spin;
    wxCustomSpinCtrl* m_Att20Spin;
    wxCustomSpinCtrl* m_Att30Spin;

    wxNotebook*     m_Notebook;

    DECLARE_EVENT_TABLE()
};

#endif
