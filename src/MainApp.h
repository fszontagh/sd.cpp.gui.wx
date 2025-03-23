#ifndef MAINAPP_H
#define MAINAPP_H

class MainApp : public wxApp {
private:
    wxSingleInstanceChecker* m_checker = nullptr;

public:
    bool OnInit() override {
        wxString forceType                  = wxEmptyString;
        bool allow_multiple_instance        = false;
        bool disableExternalProcessHandling = false;
        this->backend                       = "cpu";

        this->initConfig();
        SetAppDisplayName(PROJECT_DISPLAY_NAME);

        std::map<std::string, std::string> backendMap = {
            {"-cuda", "cuda"},
            {"-avx", "avx"},
            {"-avx2", "avx2"},
            {"-avx512", "avx512"},
            {"-hipblas", "hipblas"},
            {"-vulkan", "vulkan"},
        };

        for (int i = 0; i < wxApp::argc; ++i) {
            wxString arg = wxApp::argv[i];
            if (backendMap.count(arg.ToStdString())) {
                forceType     = backendMap[arg.ToStdString()];
                this->backend = forceType;
            } else if (arg == "-allow-multiple") {
                allow_multiple_instance = true;
            } else if (arg == "-disable-external-process-handling") {
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
        splash_bitmap.SetMask(new wxMask(splash_bitmap, wxColour(0u, 0u, 0u)));
        wxSplashScreen* splash = new wxSplashScreen(
            splash_bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT, 6000, nullptr, -1,
            wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_SHAPED);
        splash->SetShape(wxRegion(splash_bitmap));
        SetTopWindow(splash);
        splash->Show();

        std::string libPrefix =
#ifdef WIN32
            "";
#else
            "lib";
#endif

        this->stableDiffusionDllName = libPrefix + "stable-diffusion";
        this->llamaDllName          = libPrefix + "llama";

        if (!forceType.empty()) {
            this->stableDiffusionDllName += "_" + forceType;
            this->llamaDllName += "_" + forceType;
        } else {
            static const cpu_features::X86Features features = cpu_features::GetX86Info().features;
            if (isNvidiaGPU()) {
                this->backend = "cuda";
            } else if (isAmdGPU() || isIntelGPU()) {
                this->backend = "vulkan";
            } else if (features.avx512_fp16 || features.avx512_bf16 || features.avx512vl) {
                this->backend = "avx512";
            } else if (features.avx2) {
                this->backend = "avx2";
            } else if (features.avx) {
                this->backend = "avx";
            }

            this->stableDiffusionDllName += "_" + this->backend;
            this->llamaDllName += "_" + this->backend;
        }

        auto configLang = this->config->Read("/language", wxUILocale::GetSystemLocaleId().GetLanguage());
        if (configLang.empty()) {
            configLang = wxUILocale::GetSystemLocaleId().GetLanguage();
        }
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
        this->mainFrame = new MainWindowUI(nullptr, this->stableDiffusionDllName.utf8_string(), this->llamaDllName.utf8_string(), this->backend.utf8_string(), disableExternalProcessHandling, this);

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
                    server->LoadAuthKeyFromSecretStore();
                    if (server->GetAuthKeyState() == true) {
                        server->StartServer();
                    } else {
                        server->SetEnabled(false);
                    }
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
    MainWindowUI* mainFrame         = nullptr;
    wxString stableDiffusionDllName = wxEmptyString;
    wxString llamaDllName          = wxEmptyString;
    wxString backend                = wxEmptyString;
    wxLocale* m_Locale              = nullptr;
};
#endif