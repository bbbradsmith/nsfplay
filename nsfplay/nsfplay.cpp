// stub

#include <wx/wx.h>
#include <cstdio> // std::vfprintf
#include <cstdarg> // va_list, va_start

// TODO these should go to logs (needed for ../shared/sound.cpp)

void err_printf(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	std::vfprintf(stderr,fmt,args);
}

void out_printf(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	std::vfprintf(stdout,fmt,args);
}

class MainApp : public wxApp
{
public:
	bool OnInit() override;
};

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit()
{
	wxMessageBox("NSFPlay 3 stub","NSFPlay 3",wxOK | wxICON_INFORMATION);
	return false;
}
