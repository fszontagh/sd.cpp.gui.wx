#include <wx/app.h>
#include <wx/event.h>
#define STB_IMAGE_IMPLEMENTATION
#include "ui/MainWindowUI.h"
#include <wx/image.h>
#include <wx/snglinst.h>
#include "ui/embedded_files/app_icon.h"
#include <wx/dynlib.h>
#include "helpers/cpuinfo_x86.hpp"
#include "helpers/vcardinfo.hpp"

// Define the MainApp
class MainApp : public wxApp
{
private:
    wxSingleInstanceChecker *m_checker;

public:
    bool OnInit() override
    {

        wxString forceType = "";
        bool allow_multiple_instance = false;

        for (int i = 0; i < wxApp::argc; ++i)
        {
            if (wxApp::argv[i] == "-cuda")
            {
                forceType = "cuda";
            }
            if (wxApp::argv[i] == "-avx")
            {
                forceType = "avx";
            }
            if (wxApp::argv[i] == "-avx2")
            {
                forceType = "avx2";
            }
            if (wxApp::argv[i] == "-avx512")
            {
                forceType = "avx512";
            }
            if (wxApp::argv[i] == "-rocm")
            {
                forceType = "rocm";
            }
            if (wxApp::argv[i] == "-allow-multiple")
            {
                allow_multiple_instance = true;
            }
        }
        if (!allow_multiple_instance)
        {
            m_checker = new wxSingleInstanceChecker;
            if (m_checker->IsAnotherRunning())
            {
                wxLogError(_("Another program instance is already running, aborting."));
                delete m_checker; // OnExit() won't be called if we return false
                m_checker = NULL;
                return false;
            }
        }

        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        wxBitmap splash_bitmap(splash_img_to_wx_bitmap());
        wxMask *mask = new wxMask(splash_bitmap, wxColour(0u, 0u, 0u));
        splash_bitmap.SetMask(mask);
        wxRegion path(splash_bitmap);

        wxSplashScreen *splash = new wxSplashScreen(splash_bitmap,
                                                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                                    6000, nullptr, -1, wxDefaultPosition, wxDefaultSize,
                                                    wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED);
        splash->SetShape(path);

        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString appPath(f.GetPath());
        wxDynamicLibrary *dll = new wxDynamicLibrary();

        wxString dllName = "stable-diffusion";

        if (!forceType.empty())
        {
            dllName = "stable-diffusion_" + forceType;
        }
        else
        {
            if (isNvidiaGPU())
            {
                dllName = "stable-diffusion_cuda";
            }
            else if (isAmdGPU())
            {
                dllName = "stable-diffusion_rocm";
            }
            else
            {
                static const cpu_features::X86Features features = cpu_features::GetX86Info().features;
                if (features.avx512_fp16 || features.avx512_bf16 || features.avx512vl)
                {
                    dllName = "stable-diffusion_avx512";
                }
                else if (features.avx2)
                {
                    dllName = "stable-diffusion_avx2";
                }
                else if (features.avx)
                {
                    dllName = "stable-diffusion_avx";
                }
            }
        }
        if (!dll->Load(dllName, wxDL_QUIET | wxDL_DEFAULT))
        {
            splash->Hide();
            splash->Destroy();
            wxMessageBox(wxString::Format(_("Can not load backend: %s"), dllName));
            exit(1);
        }

        MainWindowUI *mainFrame = new MainWindowUI(nullptr);
        mainFrame->loadDll(dll);
        mainFrame->Show(true);
        SetTopWindow(mainFrame);
        splash->Destroy();
        return true;
    }
    int OnExit() override
    {
        delete m_checker;
        return 0;
    }
};

DECLARE_APP(MainApp)
wxIMPLEMENT_APP(MainApp);