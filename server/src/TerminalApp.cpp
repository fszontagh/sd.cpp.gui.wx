#include "TerminalApp.h"

bool TerminalApp::OnInit() {
    wxLog::SetTimestamp("%Y-%m-%d %H:%M:%S");

    if (argc < 2) {
        wxLogError("Usage: %s <config.json>", argv[0]);
        return false;
    }

    wxFileName config(argv[1]);
    if (config.FileExists() == false) {
        wxLogError("Config file not found: %s", argv[1]);
        return false;
    }

    wxFile file;
    wxString fileData;

    if (file.Open(config.GetAbsolutePath())) {
        file.ReadAll(&fileData);
        file.Close();
    }else{
        wxLogError("Failed to open config file: %s", config.GetAbsolutePath());
        return false;
    }
    

    try {
        nlohmann::json cfg = nlohmann::json::parse(fileData.utf8_string());
        auto cdata         = cfg.get<ServerConfig>();
        this->configData   = std::make_shared<ServerConfig>(cdata);
    } catch (const nlohmann::json::parse_error& e) {
        wxLogError("Error parsing config file: %s", e.what());
        return false;
    }
    // set a random identifier to the server if not set in the config
    if (this->configData->authkey.empty()) {
        this->configData->authkey = std::to_string(std::rand());
        this->configData->authkey = sd_gui_utils::sha256_string_openssl(this->configData->authkey);
        wxLogInfo("Generated authkey: %s", this->configData->authkey);
        // save into the config file
        try {
            nlohmann::json saveJsonData = *this->configData.get();
            wxFile file;
            if (file.Open(config.GetAbsolutePath(), wxFile::write)) {
                file.Write(saveJsonData.dump(4));
                file.Close();                
            }else{
                wxLogError("Failed to open config file: %s", config.GetAbsolutePath());
            }

        } catch (const nlohmann::json::parse_error& e) {
            wxLogError("Error parsing config file: %s", e.what());
            return false;
        }
    }
    if (configData->logfile.empty() == false) {
        configData->logfile = wxFileName(configData->logfile).GetAbsolutePath();
        wxLogInfo("Logging to file: %s", configData->logfile);

        this->logfile = std::fopen(configData->logfile.c_str(), "a+");
        if (this->logfile == nullptr) {
            wxLogError("Failed to open log file: %s", configData->logfile);
            return false;
        }
        this->logger = new wxLogStderr(this->logfile);
        wxASSERT(this->logger != nullptr);
        this->oldLogger = wxLog::GetActiveTarget();
        wxLog::SetActiveTarget(this->logger);
    }
    this->sharedMemoryManager = std::make_shared<SharedMemoryManager>(SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE, true);
    wxLogDebug(wxString::Format("Shared memory initialized: %s size: %d", SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE));
    // init events
    Bind(wxEVT_THREAD_LOG, [](wxCommandEvent& evt) {
        switch (evt.GetInt()) {
            case wxLOG_Info:
                wxLogInfo(evt.GetString());
                break;
            case wxLOG_Warning:
                wxLogWarning(evt.GetString());
                break;
            case wxLOG_Error:
                wxLogError(evt.GetString());
                break;
            case wxLOG_Debug:
                wxLogDebug(evt.GetString());
                break;
            default:
                wxLogInfo(evt.GetString());
                break;
        }
    });

    Bind(wxEVT_TIMER, [this](wxTimerEvent& evt) {
        if (this->socket != nullptr) {
            this->socket->OnTimer();
        }
    });

    this->timer.SetOwner(this, wxID_ANY);
    this->timer.Start(1000);

    this->eventHandlerReady = true;
    // find the external process
    wxString currentPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
    wxFileName f(currentPath, "");
    f.SetName(EPROCESS_BINARY_NAME);

    if (f.Exists() == false) {
        f.AppendDir("extprocess");
    }
    if (f.Exists() == false) {
        f.AppendDir("Debug");
    }

    this->extprocessCommand = f.GetFullPath();
    std::string dllName     = "stable-diffusion_";
    dllName += backend_type_to_str.at(this->configData->backend);

    // get the library
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    wxString dllFullPath  = currentPath + wxFileName::GetPathSeparator() + wxString::FromUTF8Unchecked(dllName.c_str());
    this->extProcessParam = dllFullPath.utf8_string() + ".dll";

    if (std::filesystem::exists(this->extProcessParam.utf8_string()) == false) {
        this->sendLogEvent("Shared lib not found: " + this->extProcessParam, wxLOG_Error);
        return false;
    }
#else
    this->extProcessParam = "lib" + dllName + ".so";
#endif
    wxLogInfo("Using lib: %s", this->extProcessParam.utf8_string());
    this->sharedLibrary = std::make_shared<SharedLibrary>(this->extProcessParam.utf8_string());
    try {
        this->sharedLibrary->load();
    } catch (std::exception& e) {
        wxLogError(e.what());
        return false;
    }
    return true;
    // return wxAppConsole::OnInit();  // Call the base class implementation
}

int TerminalApp::OnExit() {
    wxLogInfo("OnExit started");

    if (this->socket != nullptr) {
        this->socket->stop();
    }

    for (auto& thread : this->threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    this->threads.clear();
    if (this->sharedLibrary != nullptr) {
        try {
            this->sharedLibrary->unload();
        } catch (std::exception& e) {
            wxLogError(e.what());
        }
    }

    wxLog::SetActiveTarget(this->oldLogger);

    wxLogDebug("Attempting to stop logger.");
    delete this->logger;
    fclose(this->logfile);

    return wxAppConsole::OnExit();
}

int TerminalApp::OnRun() {
    wxEventLoop loop;
    wxEventLoopBase::SetActive(&loop);

    std::thread tr([this]() {
        while (!this->eventHandlerReady) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        this->sendLogEvent("Starting socket server", wxLOG_Info);
        try {
            this->socket = new SocketApp(this->configData->host.c_str(), this->configData->port, this);
        } catch (std::exception& e) {
            this->sendLogEvent(e.what(), wxLOG_Error);
        }
        

        while (this->socket->isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        delete this->socket;
        this->socket = nullptr;
        this->sendLogEvent("Socket server stopped", wxLOG_Info);
    });

    this->threads.emplace_back(std::move(tr));

    return wxAppConsole::OnRun();
}