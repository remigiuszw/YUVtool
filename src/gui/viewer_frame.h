#ifndef VIEWER_FRAME_H
#define VIEWER_FRAME_H

#include "wx/wx.h"

class Viewer_frame : public wxFrame
{
public:
  Viewer_frame(const wxString& title);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

private:
  DECLARE_EVENT_TABLE()
};

#endif // VIEWER_FRAME_H
