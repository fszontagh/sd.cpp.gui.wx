#ifndef MAINAPP_H
#define MAINAPP_H

class MainApp : public wxApp {
private:
    wxSingleInstanceChecker* m_checker;

public:
    bool OnInit() override {
        wxString forceType                  = "";
        bool allow_multiple_instance        = false;
        this->backend                       = "cpu";
        bool disableExternalProcessHandling = false;

        this->initConfig();
        SetAppDisplayName(PROJECT_DISPLAY_NAME);

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
            if (wxApp::argv[i] == "-vulkan") {
                forceType     = "vulkan";
                this->backend = "vulkan";
            }
            if (wxApp::argv[i] == "-allow-multiple") {
                allow_multiple_instance = true;
            }
            if (wxApp::argv[i] == "-disable-external-process-handling") {
                disableExternalProcessHandling = true;
                std::cout << "External process handling disabled" << std::endl;
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
                                                    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
                                                    6000, nullptr, -1, wxDefaultPosition, wxDefaultSize,
                                                    wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED);
        splash->SetShape(path);
        SetTopWindow(splash);
        splash->Show();

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
                dllName       = libPrefix + "stable-diffusion_vulkan";
                this->backend = "vulkan";
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

        auto configLang = this->config->Read("/language", wxUILocale::GetSystemLocaleId().GetLanguage());
        // if empty, use system (empty while edited with manually the ini file)
        if (configLang.empty()) {
            configLang = wxUILocale::GetSystemLocaleId().GetLanguage();
        }

        // trans->SetLanguage(configLang);
        this->ReloadMainWindow(configLang, disableExternalProcessHandling);
        splash->Destroy();
        return true;
    }
    int OnExit() override {
        delete m_checker;
        if (this->cfg != nullptr) {
            delete this->cfg;
        }
        return 0;
    }
    void ReloadMainWindow(const wxString& newLangName, bool disableExternalProcessHandling = false) {
        if (this->mainFrame != nullptr) {
            SetTopWindow(NULL);
            this->mainFrame->Destroy();
        }
        this->ChangeLocale(newLangName);
        this->mainFrame = new MainWindowUI(nullptr, this->dllName.utf8_string(), this->backend.utf8_string(), disableExternalProcessHandling, this);

        // load models
        this->mainFrame->LoadPresets();
        this->mainFrame->LoadPromptTemplates();
        this->mainFrame->loadModelList();
        this->mainFrame->loadLoraList();
        this->mainFrame->loadVaeList();
        this->mainFrame->loadTaesdList();
        this->mainFrame->loadControlnetList();
        this->mainFrame->loadEsrganList();
        this->mainFrame->loadEmbeddingList();
        this->mainFrame->loadSchedulerList();
        this->mainFrame->loadSamplerList();
        this->mainFrame->loadTypeList();

        SetTopWindow(this->mainFrame);
        this->mainFrame->Show();
        if (this->cfg->initServerList(this->mainFrame->GetEventHandler())) {
            if (this->cfg->servers.empty() == false) {
                for (auto& server : this->cfg->ListRemoteServers()) {
                    if (server->IsEnabled() == false) {
                        continue;
                    }
                    server->StartServer();
                }
            }
        }
    };

    void ChangeLocale(const wxString& newLangName) {
        wxLanguageInfo info;
        auto linfo = wxUILocale::FindLanguageInfo(newLangName);

        if (linfo != nullptr) {
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
    }

    void inline initConfig() {
        if (this->config != nullptr) {
            this->config->Flush();
            delete this->config;
            this->config = nullptr;
        }
        this->config = new wxConfig(PROJECT_NAME, SD_GUI_AUTHOR, wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
        this->cfg    = new sd_gui_utils::config(this->config);
        wxConfigBase::Set(this->config);
    }

    wxConfigBase* config      = nullptr;
    sd_gui_utils::config* cfg = nullptr;

private:
    MainWindowUI* mainFrame = nullptr;
    wxString dllName;
    wxString backend;
    wxLocale* m_Locale = nullptr;
};
#endif