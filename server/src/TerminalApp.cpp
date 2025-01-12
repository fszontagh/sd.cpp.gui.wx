#include "TerminalApp.h"
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
        nlohmann::json cfg = nlohmann::json::parse(fileData.utf8_string(), nullptr, true, true);
        auto cdata         = cfg.get<ServerConfig>();
        this->configData   = std::make_shared<ServerConfig>(cdata);
    } catch (const nlohmann::json::parse_error& e) {
        wxLogError("Error parsing config file: %s", e.what());
        return false;
    }
    // set a random identifier to the server if not set in the config
    if (this->configData->authkey.empty()) {
        this->configData->authkey = sd_gui_utils::sha256_string_openssl(std::to_string(std::rand()) + this->configData->host + std::to_string(this->configData->port));
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
    if (this->configData->server_id.empty()) {
        this->configData->server_id = std::to_string(std::rand());
        this->configData->server_id = sd_gui_utils::sha256_string_openssl(this->configData->authkey + this->configData->host + this->configData->model_path);
        wxLogInfo("Generated server_id: %s", this->configData->server_id);
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
    if (configData->shared_memory_path.empty()) {
        configData->shared_memory_path = std::string(SHARED_MEMORY_PATH) + this->configData->server_id;
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

    if (this->configData->model_path.empty()) {
        wxLogError("`model_path` config key is missing");
        return false;
    }

    this->sharedMemoryManager = std::make_shared<SharedMemoryManager>(configData->shared_memory_path, SHARED_MEMORY_SIZE, true);
    wxLogDebug(wxString::Format("Shared memory initialized: %s size: %d", configData->shared_memory_path, SHARED_MEMORY_SIZE));

    Bind(wxEVT_TIMER, [this](wxTimerEvent& evt) {
        if (this->socket != nullptr) {
            this->socket->OnTimer();
        }
        evt.Skip();
    });

    this->timer.SetOwner(this, wxID_ANY);
    this->timer.Start(1000);

    this->eventHandlerReady.store(true);

    wxString currentPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();

    if (this->configData->exprocess_binary_path.empty() == false) {
        this->extprocessCommand = wxFileName(this->configData->exprocess_binary_path).GetAbsolutePath();
    } else {
        wxFileName f(currentPath, "");
        f.SetName(EPROCESS_BINARY_NAME);

        if (f.Exists() == false) {
            wxLogWarning("External process not found: %s", f.GetFullPath().utf8_string());
            f.AppendDir("extprocess");
        }
        if (f.Exists() == false) {
            wxLogWarning("External process not found: %s", f.GetFullPath().utf8_string());
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
    wxString dllFullPath = currentPath + wxFileName::GetPathSeparator() + wxString::FromUTF8Unchecked(dllName.c_str());
    this->extProcessParams.Add(dllFullPath.utf8_string() + ".dll");

    if (wxFileExists(this->extProcessParams.begin()->c_str()) == false) {
        this->sendLogEvent("Shared lib not found: " + this->extProcessParams.begin()->c_str(), wxLOG_Error);
        return false;
    }
#else
    this->extProcessParams.Add("lib" + dllName + ".so");

#endif
    wxLogInfo("Using lib: %s", this->extProcessParams.begin()->c_str());
    this->sharedLibrary = std::make_shared<SharedLibrary>(this->extProcessParams.begin()->ToStdString());
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
    // let the hashing begin
    this->CalcModelHashes();

    this->extProcessParams.Add(this->configData->shared_memory_path);
    this->extProcessNeedToRun.store(true);
    return true;
    // return wxAppConsole::OnInit();  // Call the base class implementation
}

int TerminalApp::OnExit() {
    wxLogInfo("Exiting...");

    if (this->socket != nullptr) {
        this->socket->stop();
    }

    if (this->subprocess != nullptr) {
        this->extProcessNeedToRun.store(false);
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
    if (this->logThread.joinable()) {
        this->logThread.join();
    }
    wxLog::SetActiveTarget(this->oldLogger);

    wxLogDebug("Stopping logger... bye!");
    delete this->logger;
    fclose(this->logfile);

    return wxAppConsole::OnExit();
}

int TerminalApp::OnRun() {
    this->logThread = std::thread(&TerminalApp::ProcessLogQueue, this);

    std::thread tr3(&TerminalApp::ProcessOutputThread, this);
    this->threads.emplace_back(std::move(tr3));

    std::thread tr([this]() {
        while (this->eventHandlerReady.load() == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        while (this->extProcessNeedToRun.load() == true && this->extprocessIsRunning.load() == false) {
            this->sendLogEvent("Waiting for external process to start", wxLOG_Info);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (this->extProcessNeedToRun.load() == false && this->extprocessIsRunning.load() == false) {
            this->sendLogEvent("External process not running, exiting...", wxLOG_Error);
            this->ExitMainLoop();
            return;
        }
        this->sendLogEvent("Starting socket server", wxLOG_Info);
        try {
            this->socket = new SocketApp(this->configData->host.c_str(), this->configData->port, this);
        } catch (std::exception& e) {
            this->sendLogEvent(e.what(), wxLOG_Error);
        }

        while (this->socket->isRunning() && this->eventHandlerReady.load() && this->extProcessNeedToRun.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        delete this->socket;
        this->socket = nullptr;
        // stop the external process too
        this->extProcessNeedToRun.store(false);
        this->sendLogEvent("Socket server stopped", wxLOG_Info);
        this->ExitMainLoop();
    });

    this->threads.emplace_back(std::move(tr));

    std::thread tr2(&TerminalApp::ExternalProcessRunner, this);
    this->threads.emplace_back(std::move(tr2));

    return wxAppConsole::OnRun();
}

void TerminalApp::ProcessOutputThread() {
    this->sendLogEvent("Starting process output monitoring thread", wxLOG_Debug);
    while (this->extProcessNeedToRun.load() == true) {
        if (this->subprocess != nullptr && subprocess_alive(this->subprocess) != 0) {
            static char stddata[1024]    = {0};
            static char stderrdata[1024] = {0};

            unsigned int size             = sizeof(stderrdata);
            unsigned int stdout_read_size = 0;
            unsigned int stderr_read_size = 0;

            stdout_read_size = subprocess_read_stdout(this->subprocess, stddata, size);
            stderr_read_size = subprocess_read_stderr(this->subprocess, stderrdata, size);

            if (stdout_read_size > 0 && std::string(stddata).find("(null)") == std::string::npos) {
                this->sendLogEvent(stddata, wxLOG_Info);
            }
            if (stderr_read_size > 0 && std::string(stderrdata).find("(null)") == std::string::npos) {
                this->sendLogEvent(stddata, wxLOG_Error);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
    }
}

void TerminalApp::ExternalProcessRunner() {
    if (this->subprocess != nullptr) {
        this->sendLogEvent("Subprocess already running", wxLOG_Error);
        return;
    }
    std::vector<const char*> command_line;
    command_line.push_back(this->extprocessCommand.c_str());
    wxString params = this->extprocessCommand;

    int counter = 1;
    for (auto const& p : this->extProcessParams) {
        params.Append(" ");
        params.Append(p);
        command_line[counter] = p.c_str();
        counter++;
    }

    command_line[counter] = nullptr;

    this->sendLogEvent(wxString::Format("Starting subprocess: %s", params), wxLOG_Info);
    this->subprocess = new subprocess_s();
    int result       = subprocess_create(command_line.data(), subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
    if (this->subprocess == nullptr || result != 0) {
        this->sendLogEvent("Failed to create subprocess", wxLOG_Error);
        this->extprocessIsRunning.store(false);
        this->extProcessNeedToRun.store(false);
        return;
    }
    this->extprocessIsRunning.store(true);
    this->sendLogEvent(wxString::Format("Subprocess created: %d", result), wxLOG_Info);

    while (this->extProcessNeedToRun.load() == true) {
        if (subprocess_alive(this->subprocess) == 0) {
            this->sendLogEvent("Subprocess stopped", wxLOG_Error);
            this->extprocessIsRunning.store(false);
            // restart
            int result = subprocess_create(command_line.data(), subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
            if (this->subprocess == nullptr) {
                this->sendLogEvent("Failed to create subprocess", wxLOG_Error);
                this->extprocessIsRunning.store(false);
                return;
            }
            this->extprocessIsRunning.store(false);
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
    this->extprocessIsRunning.store(false);
    subprocess_terminate(this->subprocess);
}

bool TerminalApp::ProcessEventHandler(std::string message) {
    if (message.empty()) {
        return false;
    }
    try {
        nlohmann::json msg = nlohmann::json::parse(message);
        sd_gui_utils::networks::Packet packet(sd_gui_utils::networks::Packet::Type::REQUEST_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_ERROR);
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
    if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST) {
        auto response = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST);
        std::vector<sd_gui_utils::networks::RemoteModelInfo> list;
        for (auto model : this->modelFiles) {
            // change the model's path
            model.second.path = this->configData->server_id + ":" + model.second.path;
            list.emplace_back(model.second);
        }
        response.SetData(list);
        this->socket->sendMsg(packet.source_idx, response);
        this->sendLogEvent("Sent model list to client: " + std::to_string(packet.source_idx), wxLOG_Info);
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
            sd_gui_utils::networks::RemoteModelInfo modelInfo(filename, sd_gui_utils::DirTypes::CHECKPOINT, this->configData->model_path);
            this->modelFiles[filename.GetAbsolutePath().utf8_string()] = modelInfo;
            used_sizes += modelInfo.size;
            used_checkpoint_sizes += modelInfo.size;
            checkpoint_count++;
            wxFileName hashFileName = filename;
            hashFileName.SetExt("sha256");
            if (!wxFileExists(hashFileName.GetFullPath())) {
                this->hashingFullSize.store(this->hashingFullSize.load() + static_cast<wxULongLong>(modelInfo.size));
            }
        }
    }
    wxLogInfo("Loaded %d checkpoints %s", checkpoint_count, wxFileName::GetHumanReadableSize(used_checkpoint_sizes));

    // load loras
    if (this->configData->lora_path.empty() == false && wxDirExists(this->configData->lora_path)) {
        wxArrayString lora_files;
        wxDir::GetAllFiles(this->configData->lora_path, &lora_files);
        int lora_count              = 0;
        wxULongLong used_lora_sizes = 0;
        for (const auto& file : lora_files) {
            wxFileName filename(file);
            if (std::find(LORA_FILE_EXTENSIONS.begin(), LORA_FILE_EXTENSIONS.end(), filename.GetExt()) != LORA_FILE_EXTENSIONS.end()) {
                sd_gui_utils::networks::RemoteModelInfo modelInfo(filename, sd_gui_utils::DirTypes::LORA, this->configData->lora_path);
                this->modelFiles[filename.GetAbsolutePath().utf8_string()] = modelInfo;
                used_sizes += modelInfo.size;
                used_lora_sizes += modelInfo.size;
                lora_count++;
                wxFileName hashFileName = filename;
                hashFileName.SetExt("sha256");
                if (!wxFileExists(hashFileName.GetFullPath())) {
                    this->hashingFullSize.store(this->hashingFullSize.load() + static_cast<wxULongLong>(modelInfo.size));
                }
            }
        }
        wxLogInfo("Loaded %d loras %s", lora_count, wxFileName::GetHumanReadableSize(used_lora_sizes));
    } else {
        wxLogWarning("Lora path does not exist or missing from config: %s", this->configData->lora_path);
    }
    wxLogInfo("Total size: %s", wxFileName::GetHumanReadableSize((wxULongLong)used_sizes));
    wxLogInfo("Need to hash: %s", wxFileName::GetHumanReadableSize(this->hashingFullSize.load()));
    return true;
}

void TerminalApp::ProcessLogQueue() {
    while (!this->eventHanlderExit.load()) {
        while (!this->eventQueue.IsEmpty()) {
            auto event = this->eventQueue.Pop();
            event();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void TerminalApp::CalcModelHashes() {
    for (auto& model : this->modelFiles) {
        wxFileName hashFileName(model.second.remote_path);
        hashFileName.SetExt("sha256");
        if (wxFileExists(hashFileName.GetFullPath())) {
            continue;
        }

        // this->sendLogEvent(wxString::Format("Calculating hash for %s size: %s (%llu bytes)", model.second.name, model.second.size_f, model.second.size), wxLOG_Info);
        wxLogInfo("Calculating hash for %s size: %s (%llu bytes)", model.second.name, model.second.size_f, model.second.size);
        model.second.hash_fullsize = model.second.size;
        *this->currentHashingItem  = model.second;
        model.second.sha256        = sd_gui_utils::sha256_file_openssl(model.second.remote_path.c_str(), (void*)this, TerminalApp::FileHashCallBack);
        wxFile file;
        if (file.Open(hashFileName.GetFullPath(), wxFile::write)) {
            wxString fileContent = wxString::Format("%s\t%s", model.second.sha256, model.second.remote_path);
            file.Write(fileContent);
            file.Close();
            wxLogInfo("Hash calculated for %s %s", model.second.path, model.second.sha256);
            this->hashingProcessed.store(this->hashingProcessed.load() + static_cast<wxULongLong>(model.second.size));
        } else {
            wxLogError("Failed to calculate hash for %s", model.second.path);
            this->hashingFullSize.store(this->hashingFullSize.load() - static_cast<wxULongLong>(model.second.size));
        }
    }
}