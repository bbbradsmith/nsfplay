// stub

#include <wx/wx.h>

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
