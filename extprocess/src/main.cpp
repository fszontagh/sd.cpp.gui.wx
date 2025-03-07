
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <dynamiclib_name> <targetlogfile.log> [shared_memory_path]" << std::endl;
        return 1;
    }

    wxString logFilePath = argv[2];
    wxFileName logFile(logFilePath);

    if (!std::filesystem::exists(logFile.GetPath().ToStdString())) {
        std::filesystem::create_directories(logFile.GetPath().ToStdString());
    }

    FILE* logfile = std::fopen(logFile.GetAbsolutePath().ToStdString().c_str(), "a+");
    if (logfile == nullptr) {
        std::cerr << "Failed to open or create log file: " << logFile.GetAbsolutePath() << std::endl;
        return 1;
    }

    auto logger = new wxLogStderr(logfile);
    logger->SetFormatter(new CustomLogFormatter());
    wxLog::SetActiveTarget(logger);
    wxLogInfo("Logging initialized. Log file: %s", logFile.GetAbsolutePath());
    wxLog::SetLogLevel(wxLOG_Max);

    std::string shared_memory_path = SHARED_MEMORY_PATH;

    if (argc > 3 && argv[3]) {
        shared_memory_path = argv[3];
    }

    wxLogInfo("Starting with shared memory size: %d", SHARED_MEMORY_SIZE);
    std::shared_ptr<SharedMemoryManager> sharedMemory = nullptr;

    try {
        sharedMemory = std::make_shared<SharedMemoryManager>(shared_memory_path.c_str(), SHARED_MEMORY_SIZE, false);
    } catch (const std::exception& e) {
        wxLogError("Failed to create SharedMemoryManager: %s", e.what());
        return 1;
    }

    if (sharedMemory == nullptr) {
        wxLogError("Failed to create SharedMemoryManager");
        return 1;
    }

    ApplicationLogic appLogic(argv[1], sharedMemory);

    if (!appLogic.loadLibrary()) {
        wxLogError("Cannot load shared library: %s", argv[1]);
        return 1;
    }

    bool needToRun  = true;
    uint64_t lastId = 0;
    while (needToRun) {
        std::unique_ptr<char[]> buffer(new char[SHARED_MEMORY_SIZE]);

        if (sharedMemory->read(buffer.get(), SHARED_MEMORY_SIZE)) {
            if (std::strlen(buffer.get()) > 0) {
                appLogic.updateLastMessageTime();
                std::string message = std::string(buffer.get(), SHARED_MEMORY_SIZE);

                if (message == "exit") {
                    wxLogWarning("Got exit command, exiting...");
                    needToRun = false;
                    break;
                }

                try {
                    nlohmann::json j = nlohmann::json::parse(message);
                    auto item        = j.get<QueueItem>();

                    if (item.id == 0) {
                        wxLogWarning("[EXTPROCESS] Got a wrong id: 0");
                    } else if (item.id != lastId && item.status == QueueStatus::PENDING) {
                        sharedMemory->clear();
                        wxLogInfo("New message: %" PRIu64, item.id);
                        lastId = item.id;
                        appLogic.processMessage(item);
                    }
                } catch (std::exception& e) {
                    wxLogError("Cannot parse JSON message: %s", e.what());
                }
            }
        } else {
            wxLogError("Cannot read shared memory");
            needToRun = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
    }

    wxLogInfo("Exiting application.");
    wxLog::FlushActive();
    std::fclose(logfile);
    return 0;
}
