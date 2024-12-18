#include "TerminalApp.h"

bool TerminalApp::OnInit() {
    wxLog::SetTimestamp("%Y-%m-%d %H:%M:%S");
    wxLogInfo("OnInit started");

    if (argc < 2) {
        wxLogError("Usage: %s <config.json>", argv[0]);
        return false;
    }

    wxFileName config(argv[1]);
    if (config.FileExists() == false) {
        wxLogError("Config file not found: %s", argv[1]);
        return false;
    }

    wxTextFile file;
    file.Open(config.GetAbsolutePath());

    wxString fileData;
    for (int i = 0; i < file.GetLineCount(); i++) {
        fileData << file.GetLine(i) + "\n";
    }
    file.Close();

    try {
        nlohmann::json cfg = nlohmann::json::parse(fileData.utf8_string());
        auto cdata         = cfg.get<ServerConfig>();
        this->configData   = std::make_shared<ServerConfig>(cdata);
    } catch (const nlohmann::json::parse_error& e) {
        wxLogError("Error parsing config file: %s", e.what());
        return false;
    }
    wxLogInfo("Config loaded");

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

    this->eventHandlerReady = true;
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

    wxLog::SetActiveTarget(this->oldLogger);

    wxLogDebug("Attempting to stop logger.");
    delete this->logger;
    fclose(this->logfile);

    return wxAppConsole::OnExit();
}

int TerminalApp::OnRun() {
    wxLogInfo("Event handler started");

    wxEventLoop loop;
    wxEventLoopBase::SetActive(&loop);

    std::thread tr([this]() {
        while (!this->eventHandlerReady) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        this->sendLogEvent("Starting thread");
        this->socket = new SocketApp(this->configData->host.c_str(), this->configData->port, this);

        while (this->socket->isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        delete this->socket;
        this->socket = nullptr;
    });

    this->threads.emplace_back(std::move(tr));

    return wxAppConsole::OnRun();
}