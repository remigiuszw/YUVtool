#ifndef YUV_TOOL_APPLICATION_H
#define YUV_TOOL_APPLICATION_H

#include "wx/wx.h"

class Yuv_tool_application : public wxApp
{
public:
  virtual bool OnInit();
};

DECLARE_APP(Yuv_tool_application)

#endif // YUV_TOOL_APPLICATION_H
