#include <wx/app.h>
#include <wx/event.h>
#define STB_IMAGE_IMPLEMENTATION
#include "MainWindowUi.h"
#include <wx/image.h>
#include "embedded_files/app_icon.h"

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);
        MainWindowUI *mainFrame = new MainWindowUI(nullptr);
        SetTopWindow(mainFrame);
        
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)