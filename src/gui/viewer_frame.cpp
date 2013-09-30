#include "viewer_frame.h"

BEGIN_EVENT_TABLE(Viewer_frame, wxFrame)
  EVT_MENU(wxID_EXIT,  Viewer_frame::OnQuit)
  EVT_MENU(wxID_ABOUT, Viewer_frame::OnAbout)
END_EVENT_TABLE()

Viewer_frame::Viewer_frame(const wxString& title) :
  wxFrame(NULL, wxID_ANY, title)
{
#if wxUSE_MENUS
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}
//------------------------------------------------------------------------------
void Viewer_frame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  Close(true);
}
//------------------------------------------------------------------------------
void Viewer_frame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
  wxMessageBox(
    wxString::Format(
      _T("Welcome to %s!\n")
      _T("\n")
      _T("This is the minimal wxWidgets sample\n")
      _T("running under %s."),
      wxVERSION_STRING,
      wxGetOsDescription().c_str()
    ),
    _T("About wxWidgets minimal sample"),
    wxOK | wxICON_INFORMATION,
    this
  );
}
