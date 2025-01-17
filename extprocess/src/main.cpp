
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dynamiclib_name> [shared_memory_path]" << std::endl;
        return 1;
    }
    std::string shared_memory_path = SHARED_MEMORY_PATH;

    if (argv[2]) {
        shared_memory_path = argv[2];
    }

    std::shared_ptr<SharedMemoryManager> sharedMemory = nullptr;

    std::cout << "[EXTPROCESS] starting with shared memory size: " << SHARED_MEMORY_SIZE << std::endl;
    try {
        sharedMemory = std::make_shared<SharedMemoryManager>(shared_memory_path.c_str(), SHARED_MEMORY_SIZE, false);
    } catch (const std::exception& e) {
        std::cerr << "[EXTPROCESS] Failed to create SharedMemoryManager: " << e.what() << std::endl;
        return 1;
    }

    if (sharedMemory == nullptr) {
        std::cerr << "[EXTPROCESS] Failed to create SharedMemoryManager" << std::endl;
        return 1;
    }

    ApplicationLogic appLogic(argv[1], sharedMemory);

    if (!appLogic.loadLibrary()) {
        std::cerr << "[EXTPROCESS] Can not load shared library" << std::endl;
        writeCriticalLog("[EXTPROCESS] Can not load shared library: " + std::string(argv[1]), "stablediffusiongui_diffuser.log");
        return 1;
    }

    bool needToRun  = true;
    uint64_t lastId = 0;
    while (needToRun) {
        std::unique_ptr<char[]> buffer(new char[SHARED_MEMORY_SIZE]);

        if (sharedMemory->read(buffer.get(), SHARED_MEMORY_SIZE)) {
            if (std::strlen(buffer.get()) > 0) {
                std::string message = std::string(buffer.get(), SHARED_MEMORY_SIZE);
                if (message == "exit") {
                    std::cout << "Got exit command, exiting... " << std::endl;
                    needToRun = false;
                    break;
                }
                try {
                    nlohmann::json j = nlohmann::json::parse(message);
                    auto item        = j.get<QueueItem>();
                    if (item.id == 0) {
                        std::cout << "[EXTPORCESS] Got a wrong id: 0" << std::endl;
                    } else if (item.id != lastId && item.status == QueueStatus::PENDING) {
                        sharedMemory->clear();
                        std::cout << "[EXTPROCESS] New message: " << item.id << std::endl;
                        lastId = item.id;
                        appLogic.processMessage(item);
                    }
                } catch (std::exception& e) {
                    std::cerr << "[EXTPROCESS] Can not parse json message: " << e.what() << std::endl;
                }
            }
        } else {
            std::cerr << "[EXTPROCESS] Can not read shared memory" << std::endl;
            needToRun = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
    }

    return 0;
}
