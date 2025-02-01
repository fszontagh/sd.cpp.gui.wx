#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

wxDECLARE_APP(TerminalApp);

class TerminalApp : public wxAppConsole {
public:
    // --- Core Methods ---
    virtual bool OnInit() override;
    virtual int OnRun() override;
    virtual int OnExit() override;
    virtual bool IsGUI() const override { return false; }

    // --- Event Handling ---
    void ProcessReceivedSocketPackages(sd_gui_utils::networks::Packet& packet);
    void SendLogEvent(wxString message, const wxLogLevel level = wxLOG_Info);
    void StoreClientStats(SocketApp::clientInfo& client);
    void SendItemUpdateEvent(std::shared_ptr<QueueItem> item);

    // --- Signal Handling ---
    static void SignalHandler(int signum) {
        if (instance) {
            instance->HandleSignal(signum);
        }
    }
    void HandleSignal(int signum) {
        this->SendLogEvent("Signal (" + std::to_string(signum) + ") received", wxLOG_Info);
        if (signum == SIGTERM || signum == SIGINT || signum == SIGKILL || signum == SIGABRT) {
            this->SendLogEvent("Shutting down...", wxLOG_Info);
            this->extProcessNeedToRun.store(false);
            this->queueNeedToRun.store(false);
            this->eventHanlderExit.store(true);
            return;
        }

        if (signum == SIGUSR1) {
            this->printStatsToLog.store(true);
        }
    }

    static TerminalApp* instance;

    // --- Job Queue & Client Handling ---
    void JobQueueThread();
    SocketApp::clientInfo ReReadClientInfo(uint64_t client_id, bool delete_old_file = false);

    // --- Configurations ---
    std::shared_ptr<ServerConfig> configData = nullptr;

private:
    // --- Internal Thread Management ---
    std::thread logThread;
    std::vector<std::thread> threads;
    std::atomic<bool> queueNeedToRun{true};
    std::atomic<bool> eventHandlerReady{false};
    std::atomic<bool> extProcessNeedToRun{false};
    std::atomic<bool> extprocessIsRunning{false};
    std::atomic<bool> eventHanlderExit{false};
    std::atomic<bool> printStatsToLog{false};

    // --- Synchronization & Locks ---
    std::mutex eventMutex;
    std::mutex logMutex;
    std::mutex processMutex;
    std::mutex clientInfoMutex;

    // --- External Process Handling ---
    void ExternalProcessRunner();
    void ExternalProcessRunnerOld();
    void ProcessOutputThread();
    void ProcessLogQueue();
    bool ProcessEventHandler(std::string message);

    std::atomic<subprocess_s*> subprocess = {nullptr};
    wxString extprocessCommand            = "";
    wxArrayString extProcessParams;

    // --- Model Handling ---
    bool LoadModelFiles();
    void CalcModelHashes();
    sd_gui_utils::RemoteModelInfo* GetModelByHash(const std::string sha256);
    std::string GetModelPathByHash(const std::string& sha256);

    std::unordered_map<std::string, sd_gui_utils::networks::RemoteModelInfo> modelFiles;

    void AddModelFile(const wxFileName& filename, const std::string& rootpath, const sd_gui_utils::DirTypes type, size_t& used_sizes, int& file_count, wxULongLong& used_model_sizes);

    std::shared_ptr<sd_gui_utils::RemoteModelInfo> currentHashingItem = nullptr;
    std::atomic<wxULongLong> hashingFullSize{0};
    std::atomic<wxULongLong> hashingProcessed{0};

    // --- Event Queue & Shared Resources ---
    EventQueue eventQueue;
    std::shared_ptr<SimpleQueueManager> queueManager                       = nullptr;
    std::shared_ptr<sd_gui_utils::SnowflakeIDGenerator> snowflakeGenerator = nullptr;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager               = nullptr;
    std::shared_ptr<SharedLibrary> sharedLibrary                           = nullptr;
    std::shared_ptr<SocketApp> socket                                      = nullptr;

    // --- Logging ---
    std::FILE* logfile  = nullptr;
    wxLogStderr* logger = nullptr;
    wxLog* oldLogger    = nullptr;

    // --- Timer & Command Line Handling ---
    wxTimer timer;
    std::vector<const char*> command_line;
    wxString params;

    // --- Static Helper Methods ---
    inline static void FileHashCallBack(size_t size, std::string name, void* data) {
        auto instance = ((TerminalApp*)data);
        size          = size == 0 ? 1 : size;
        int progress  = static_cast<int>(100.0 * size / instance->currentHashingItem->size);

        if (size % (1024 * 1024) == 0) {
            std::cout << "\33[2K\r" << std::flush;  // from win10 console
            wxULongLong currentTotal    = instance->hashingProcessed.load() + size;
            wxULongLong currentRequired = instance->hashingFullSize.load();
            auto total_progress         = wxULongLong(100) * currentTotal / currentRequired;
            auto msg                    = wxString::Format(
                "[%s - %s]: %d%% (%llu/%llu) Total: %s%%",
                instance->currentHashingItem->name,
                instance->currentHashingItem->size_f,
                progress,
                size,
                instance->currentHashingItem->size,
                total_progress.ToString());

            std::cout << "\r" << msg.ToStdString() << std::flush;
        }
    }
};

#endif  // _SERVER_TERMINALAPP_H
