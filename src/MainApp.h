#ifndef MAINAPP_H
#define MAINAPP_H

class MainApp : public wxApp {
private:
    wxSingleInstanceChecker* m_checker;

public:
    const wxString getIniPath() { return this->iniPath; }
    bool OnInit() override {
        wxString forceType                  = "";
        bool allow_multiple_instance        = false;
        this->backend                       = "cpu";
        bool disableExternalProcessHandling = false;
        this->iniPath                       = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "sd.ui.config.ini";

        this->initConfig();

        for (int i = 0; i < wxApp::argc; ++i) {
            if (wxApp::argv[i] == "-cuda") {
                forceType     = "cuda";
                this->backend = "cuda";
            }
            if (wxApp::argv[i] == "-avx") {
                forceType     = "avx";
                this->backend = "avx";
            }
            if (wxApp::argv[i] == "-avx2") {
                forceType     = "avx2";
                this->backend = "avx2";
            }
            if (wxApp::argv[i] == "-avx512") {
                forceType     = "avx512";
                this->backend = "avx512";
            }
            if (wxApp::argv[i] == "-hipblas") {
                forceType     = "hipblas";
                this->backend = "hipblas";
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
        SetTopWindow(splash);

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

        this->dllName = libPrefix + "stable-diffusion";

        if (!forceType.empty()) {
            dllName = libPrefix + "stable-diffusion_" + forceType;
        } else {
            if (isNvidiaGPU()) {
                dllName       = libPrefix + "stable-diffusion_cuda";
                this->backend = "cuda";
            } else if (isAmdGPU()) {
                dllName       = libPrefix + "stable-diffusion_hipblas";
                this->backend = "hipblas";
            } else {
                static const cpu_features::X86Features features = cpu_features::GetX86Info().features;
                if (features.avx512_fp16 || features.avx512_bf16 || features.avx512vl) {
                    dllName       = libPrefix + "stable-diffusion_avx512";
                    this->backend = "avx512";
                } else if (features.avx2) {
                    dllName       = libPrefix + "stable-diffusion_avx2";
                    this->backend = "avx2";
                } else if (features.avx) {
                    dllName       = libPrefix + "stable-diffusion_avx";
                    this->backend = "avx";
                }
            }
        }

        // load locale

        auto configLang = this->config->Read("language", wxUILocale::GetLanguageInfo(wxUILocale::GetSystemLocale())->CanonicalName.utf8_string());
        // if empty, use system (empty while edited with manually the ini file)
        if (configLang.empty()) {
            configLang = wxUILocale::GetLanguageInfo(wxUILocale::GetSystemLocale())->CanonicalName.utf8_string();
        }

        // trans->SetLanguage(configLang);
        this->ReloadMainWindow(configLang);
        splash->Destroy();
        return true;
    }
    int OnExit() override {
        delete m_checker;
        return 0;
    }
    void ReloadMainWindow(wxString newLangName) {
        if (this->mainFrame != nullptr) {
            SetTopWindow(NULL);
            this->mainFrame->Destroy();
        }

        wxLanguageInfo info;
        auto linfo = wxUILocale::FindLanguageInfo(newLangName);

        // const auto best = wxTranslations::Get()->GetBestTranslation("stablediffusiongui", newLangName);
        // std::cout << "Best translation: " << best.utf8_string() << std::endl;

        if (linfo != nullptr) {
            std::cout << "Lang tag: " << linfo->GetCanonicalWithRegion() << ", " << linfo->CanonicalName.utf8_string() << std::endl;
            // wxUILocale::GetCurrent().UseLocaleName("en_US.utf8");
            // wxTranslations::Get()->SetLanguage("en_US.utf8");
            // wxUILocale::GetCurrent().FromTag(best);
            // wxTranslations::Get()->SetLanguage(best);

            if (this->m_Locale != nullptr) {
                delete this->m_Locale;
                this->m_Locale = nullptr;
            }
            this->m_Locale = new wxLocale;
            this->m_Locale->Init(linfo->GetLocaleName(), newLangName);
            this->m_Locale->AddCatalogLookupPathPrefix(".");
            if (std::filesystem::exists("./locale")) {
                this->m_Locale->AddCatalogLookupPathPrefix("./locale");
            }
            if (std::filesystem::exists("/usr/share/locale")) {
                this->m_Locale->AddCatalogLookupPathPrefix("/usr/share/locale");
            }

#ifdef USE_COREUTILS_MO
            this->m_Locale->AddCatalog("coreutils");
#endif  // USE_COREUTILS_MO
            this->m_Locale->AddCatalog("stablediffusiongui");
        }

        this->mainFrame = new MainWindowUI(nullptr, this->dllName.utf8_string(), this->backend.utf8_string(), this->disableExternalProcessHandling, this);
        SetTopWindow(this->mainFrame);
        this->mainFrame->Centre();
        this->mainFrame->Show();
    };

    void inline initConfig() {
        if (this->config) {
            wxDELETE(this->config);
        }
        this->config = new wxFileConfig(PROJECT_NAME, SD_GUI_AUTHOR, this->getIniPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
        this->config->SetVendorName(SD_GUI_AUTHOR);
        this->config->SetAppName(PROJECT_NAME);
        this->config->DisableAutoSave();
        wxConfigBase::Set(this->config);
    }

    wxFileConfig* config = nullptr;

private:
    MainWindowUI* mainFrame = nullptr;
    wxString dllName;
    wxString backend;
    bool disableExternalProcessHandling = false;
    wxString iniPath;
    wxLocale* m_Locale = nullptr;
};
#endif