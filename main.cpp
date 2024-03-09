#include <wx/app.h>
#include <wx/event.h>
#define STB_IMAGE_IMPLEMENTATION
#include "MainWindowUI.h"
#include <wx/image.h>
#include <wx/snglinst.h>
#include "embedded_files/app_icon.h"

// Define the MainApp
class MainApp : public wxApp
{
private:
    wxSingleInstanceChecker *m_checker;

public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
        m_checker = new wxSingleInstanceChecker;
        if (m_checker->IsAnotherRunning())
        {
            wxLogError(_("Another program instance is already running, aborting."));
            delete m_checker; // OnExit() won't be called if we return false
            m_checker = NULL;
            return false;
        }
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);
        MainWindowUI *mainFrame = new MainWindowUI(nullptr);

        return mainFrame->Show();
    }
    int MainApp::OnExit()
    {
        delete m_checker;
        return 0;
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
