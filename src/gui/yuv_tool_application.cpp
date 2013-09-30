#include "yuv_tool_application.h"
#include "viewer_frame.h"

IMPLEMENT_APP(Yuv_tool_application)

bool Yuv_tool_application::OnInit() {
  if ( !wxApp::OnInit() )
      return false;

  Viewer_frame *viewer_frame = new Viewer_frame(_T("YUV viewer"));
  viewer_frame->Show(true);

  return true;
}
