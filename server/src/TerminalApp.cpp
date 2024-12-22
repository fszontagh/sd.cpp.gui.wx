#include "TerminalApp.h"
#include <wx/dir.h>
#include "libs/subprocess.h"

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
    } else {
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
            } else {
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
    // find the external process if not in the config
    if (this->configData->exprocess_binary_path.empty() == false) {
        this->extprocessCommand = wxFileName(this->configData->exprocess_binary_path).GetAbsolutePath();
    } else {
        wxString currentPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
        wxFileName f(currentPath, "");
        f.SetName(EPROCESS_BINARY_NAME);

        if (f.Exists() == false) {
            f.AppendDir("extprocess");
        }
        if (f.Exists() == false) {
            f.AppendDir("Debug");
        }

        if (f.Exists() == false) {
            wxLogError("External process not found: %s", f.GetFullPath().utf8_string());
            return false;
        }
        this->extprocessCommand = f.GetAbsolutePath();
    }
    if (wxFileName::Exists(this->extprocessCommand) == false) {
        wxLogError("External process not found: %s", this->extprocessCommand.utf8_string());
        return false;
    }
    std::string dllName = "stable-diffusion_";
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

    // load models
    if (this->LoadModelFiles() == false) {
        std::cerr << "Failed to load model files" << std::endl;
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

    if (this->subprocess != nullptr) {
        this->extProcessNeedToRun = false;
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

    std::thread tr2(&TerminalApp::ExternalProcessRunner, this);
    this->threads.emplace_back(std::move(tr2));

    return wxAppConsole::OnRun();
}

void TerminalApp::ExternalProcessRunner() {
    if (this->subprocess != nullptr) {
        this->sendLogEvent("Subprocess already running", wxLOG_Error);
        return;
    }
    this->sendLogEvent(wxString::Format("Starting subprocess: %s %s", this->extprocessCommand.utf8_string().c_str(), this->extProcessParam.utf8_string().c_str()), wxLOG_Info);
    const char* command_line[] = {this->extprocessCommand.c_str(), this->extProcessParam.c_str(), nullptr};
    this->subprocess           = new subprocess_s();
    int result                 = subprocess_create(command_line, subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
    if (this->subprocess == nullptr) {
        this->sendLogEvent("Failed to create subprocess", wxLOG_Error);
        this->extprocessIsRunning = false;
        return;
    }
    this->sendLogEvent(wxString::Format("Subprocess created: %d", result), wxLOG_Info);

    while (this->extProcessNeedToRun == true) {
        if (subprocess_alive(this->subprocess) == 0) {
            this->sendLogEvent("Subprocess stopped", wxLOG_Error);
            this->extprocessIsRunning = false;
            // restart
            int result = subprocess_create(command_line, subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
            if (this->subprocess == nullptr) {
                this->sendLogEvent("Failed to create subprocess", wxLOG_Error);
                this->extprocessIsRunning = false;
                return;
            }
            this->extprocessIsRunning = true;
            this->sendLogEvent(wxString::Format("Subprocess created: %d", result), wxLOG_Info);
        }
        // handle shared memory

        std::unique_ptr<char[]> buffer(new char[SHARED_MEMORY_SIZE]);

        this->sharedMemoryManager->read(buffer.get(), SHARED_MEMORY_SIZE);

        if (std::strlen(buffer.get()) > 0) {
            bool state = this->ProcessEventHandler(std::string(buffer.get(), std::strlen(buffer.get())));
            if (state == true) {
                this->sharedMemoryManager->clear();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    this->extprocessIsRunning = false;
    subprocess_terminate(this->subprocess);
}

bool TerminalApp::ProcessEventHandler(std::string message) {
    if (message.empty()) {
        return false;
    }
    try {
        nlohmann::json msg = nlohmann::json::parse(message);
        sd_gui_utils::networks::Packet packet(sd_gui_utils::networks::PacketType::REQUEST, sd_gui_utils::networks::PacketParam::ERROR);
        packet.SetData(message);
        this->socket->sendMsg(0, packet);

    } catch (const std::exception& e) {
        this->sendLogEvent(e.what(), wxLOG_Error);
    }

    return false;
}

void TerminalApp::ProcessReceivedSocketPackages(const sd_gui_utils::networks::Packet& packet) {
    if (packet.source_idx == -1) {
        this->sendLogEvent("Invalid source index", wxLOG_Error);
        return;
    }
    if (packet.param == sd_gui_utils::networks::PacketParam::MODEL_LIST) {
        auto response = sd_gui_utils::networks::Packet(sd_gui_utils::networks::PacketType::RESPONSE, sd_gui_utils::networks::PacketParam::MODEL_LIST);
        std::vector<sd_gui_utils::networks::RemoteModelInfo> list;
        for (const auto model : this->modelFiles) {
            list.emplace_back(model.second);
        }
        response.SetData(list);
        this->socket->sendMsg(packet.source_idx, response);
    }
}

bool TerminalApp::LoadModelFiles() {
    // this->sendLogEvent("Loading model files", wxLOG_Info);
    wxLogInfo("Loading model files");
    size_t used_sizes = 0;
    // load checkpoints
    if (this->configData->model_path.empty() || wxDirExists(this->configData->model_path) == false) {
        wxLogError("Model path does not exist: %s", this->configData->model_path);
        return false;
    }

    wxArrayString checkpoint_files;
    wxDir::GetAllFiles(this->configData->model_path, &checkpoint_files);
    int checkpoint_count              = 0;
    wxULongLong used_checkpoint_sizes = 0;
    for (const auto& file : checkpoint_files) {
        wxFileName filename(file);
        if (std::find(CHECKPOINT_FILE_EXTENSIONS.begin(), CHECKPOINT_FILE_EXTENSIONS.end(), filename.GetExt()) != CHECKPOINT_FILE_EXTENSIONS.end()) {
            sd_gui_utils::networks::RemoteModelInfo modelInfo          = sd_gui_utils::networks::RemoteModelInfo(filename, sd_gui_utils::DirTypes::CHECKPOINT);
            this->modelFiles[filename.GetAbsolutePath().utf8_string()] = modelInfo;
            used_sizes += modelInfo.size;
            used_checkpoint_sizes += modelInfo.size;
            checkpoint_count++;
        }
    }
    wxLogInfo("Loaded %d checkpoints %s", checkpoint_count, wxFileName::GetHumanReadableSize(used_checkpoint_sizes));

    // load loras
    wxArrayString lora_files;
    wxDir::GetAllFiles(this->configData->lora_path, &lora_files);
    int lora_count              = 0;
    wxULongLong used_lora_sizes = 0;
    for (const auto& file : lora_files) {
        wxFileName filename(file);
        if (std::find(LORA_FILE_EXTENSIONS.begin(), LORA_FILE_EXTENSIONS.end(), filename.GetExt()) != LORA_FILE_EXTENSIONS.end()) {
            sd_gui_utils::networks::RemoteModelInfo modelInfo          = sd_gui_utils::networks::RemoteModelInfo(filename, sd_gui_utils::DirTypes::LORA);
            this->modelFiles[filename.GetAbsolutePath().utf8_string()] = modelInfo;
            used_sizes += modelInfo.size;
            used_lora_sizes += modelInfo.size;
            lora_count++;
        }
    }
    wxLogInfo("Loaded %d loras %s", lora_count, wxFileName::GetHumanReadableSize(used_lora_sizes));
    wxLogInfo("Total size: %s", wxFileName::GetHumanReadableSize((wxULongLong)used_sizes));
    return true;
}