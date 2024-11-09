#include <wx/app.h>
#include <wx/dynlib.h>
#include <wx/event.h>
#include <wx/image.h>
#include <wx/snglinst.h>
#include <csignal>
#include <iostream>
#include <ostream>
#include "helpers/cpuinfo_x86.hpp"
#include "helpers/vcardinfo.hpp"
#include "ui/MainWindowUI.h"
#include "ui/embedded_files/splash_image.h"
#include "wx/intl.h"
#include "wx/uilocale.h"

// Define the MainApp
class MainApp : public wxApp {
private:
    wxSingleInstanceChecker* m_checker;

public:
    bool OnInit() override {
        wxString forceType                  = "";
        bool allow_multiple_instance        = false;
        std::string usingBackend            = "cpu";
        bool disableExternalProcessHandling = false;

        for (int i = 0; i < wxApp::argc; ++i) {
            if (wxApp::argv[i] == "-cuda") {
                forceType    = "cuda";
                usingBackend = "cuda";
            }
            if (wxApp::argv[i] == "-avx") {
                forceType    = "avx";
                usingBackend = "avx";
            }
            if (wxApp::argv[i] == "-avx2") {
                forceType    = "avx2";
                usingBackend = "avx2";
            }
            if (wxApp::argv[i] == "-avx512") {
                forceType    = "avx512";
                usingBackend = "avx512";
            }
            if (wxApp::argv[i] == "-hipblas") {
                forceType    = "hipblas";
                usingBackend = "hipblas";
            }
            if (wxApp::argv[i] == "-allow-multiple") {
                allow_multiple_instance = true;
            }
            if (wxApp::argv[i] == "-disable-external-process-handling") {
                disableExternalProcessHandling = true;
            }
        }
        if (!allow_multiple_instance) {
            m_checker = new wxSingleInstanceChecker;
            if (m_checker->IsAnotherRunning()) {
                wxLogError(_("Another program instance is already running, aborting."));
                this->OnExit();
                return false;
            }
        }

        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        wxBitmap splash_bitmap(splash_img_to_wx_bitmap());
        wxMask* mask = new wxMask(splash_bitmap, wxColour(0u, 0u, 0u));
        splash_bitmap.SetMask(mask);
        wxRegion path(splash_bitmap);

        wxSplashScreen* splash = new wxSplashScreen(splash_bitmap,
                                                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                                                    6000, nullptr, -1, wxDefaultPosition, wxDefaultSize,
                                                    wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED);
        splash->SetShape(path);

        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString appPath(f.GetPath());
        std::string libPrefix = "";
#ifdef WIN32
        libPrefix = "";
#elif defined(__linux__)
        libPrefix = "lib";
#elif defined(__APPLE__)
        libPrefix = "lib";
#endif

        wxString dllName = libPrefix + "stable-diffusion";

        if (!forceType.empty()) {
            dllName = libPrefix + "stable-diffusion_" + forceType;
        } else {
            if (isNvidiaGPU()) {
                dllName      = libPrefix + "stable-diffusion_cuda";
                usingBackend = "cuda";
            } else if (isAmdGPU()) {
                dllName      = libPrefix + "stable-diffusion_hipblas";
                usingBackend = "hipblas";
            } else {
                static const cpu_features::X86Features features = cpu_features::GetX86Info().features;
                if (features.avx512_fp16 || features.avx512_bf16 || features.avx512vl) {
                    dllName      = libPrefix + "stable-diffusion_avx512";
                    usingBackend = "avx512";
                } else if (features.avx2) {
                    dllName      = libPrefix + "stable-diffusion_avx2";
                    usingBackend = "avx2";
                } else if (features.avx) {
                    dllName      = libPrefix + "stable-diffusion_avx";
                    usingBackend = "avx";
                }
            }
        }


        // load locale

        // locales files are in the same directory as the executable
        wxFileTranslationsLoader::AddCatalogLookupPathPrefix(".");
#ifdef USE_COREUTILS_MO

        g_loadedCoreutilsMO = trans->AddCatalog("coreutils");
#endif  // USE_COREUTILS_MO

        const wxLanguageInfo* const langInfo = wxUILocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
        const wxString langDesc              = langInfo ? langInfo->Description : wxString("the default system language");

        wxUILocale::UseDefault();
        auto locale                 = wxUILocale::GetCurrent();
        wxTranslations* const trans = new wxTranslations();
        wxTranslations::Set(trans);

        if ( !trans->AddCatalog("stablediffusiongui") )
        {
            std::cerr << "Couldn't find/load 'stablediffusiongui' catalog for " << langDesc << std::endl;
        }

        // load locale

        MainWindowUI* mainFrame = new MainWindowUI(nullptr, dllName.ToStdString(), usingBackend, disableExternalProcessHandling);
        SetTopWindow(mainFrame);

        if (mainFrame->IsBeingDeleted() == false) {
            splash->Hide();
            mainFrame->Show(true);
            splash->Destroy();
            return true;
        }
        splash->Destroy();
        return false;
    }
    int OnExit() override {
        delete m_checker;
        return 0;
    }
};

DECLARE_APP(MainApp)
wxIMPLEMENT_APP(MainApp);