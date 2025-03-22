#include "ApplicationLogic.h"

ApplicationLogic::ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager)
    : sd_dll(libName), sharedMemoryManager(sharedMemoryManager) {
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    auto TempDir   = std::getenv("Temp");
    this->tempPath = std::filesystem::path(TempDir).string();
#else
    this->tempPath = std::filesystem::temp_directory_path().string();
#endif
}

ApplicationLogic::~ApplicationLogic() {
}

bool ApplicationLogic::loadLibrary() {
    try {
        sd_dll.load();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load shared library: " << e.what() << std::endl;
        this->error = "Failed to load shared library: " + sd_dll.getLibraryPath();
        return false;
    }
}
