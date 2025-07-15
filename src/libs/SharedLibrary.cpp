#include "SharedLibrary.h"

SharedLibrary::SharedLibrary(const std::string& libraryPath)
    : libraryPath(libraryPath), handle(nullptr) {}

SharedLibrary::~SharedLibrary() {
    if (handle) {
        unload();
    }
}

void SharedLibrary::load() {
    if (handle) {
        throw std::runtime_error("Library is already loaded.");
    }

    std::vector<std::string> tryPaths;

    if (std::filesystem::exists(libraryPath)) {
        tryPaths.push_back(libraryPath);
    }

    for (const auto& path : searchPaths) {
        std::filesystem::path fullPath = std::filesystem::path(path) / libraryPath;
        if (std::filesystem::exists(fullPath)) {
            tryPaths.push_back(fullPath.string());
        }
    }

    tryPaths.push_back(libraryPath);

    std::string lastError;

    for (const auto& libpath : tryPaths) {
#if defined(_WIN32) || defined(_WIN64)
        handle = LoadLibraryA(libpath.c_str());
#else
        handle = dlopen(libpath.c_str(), RTLD_LAZY);
#endif
        if (handle)
            return;  // siker
#if !defined(_WIN32)
        lastError = dlerror();
#endif
    }

    throw std::runtime_error("Failed to load library: " + libraryPath +
                             "\nLast error: " + lastError);
}

void SharedLibrary::unload() {
    if (!handle) {
        throw std::runtime_error("Library is not loaded.");
    }

#if defined(_WIN32) || defined(_WIN64)
    if (!FreeLibrary(static_cast<HMODULE>(handle))) {
        throw std::runtime_error("Failed to unload library: " + libraryPath);
    }
#else
    if (dlclose(handle) != 0) {
        throw std::runtime_error("Failed to unload library: " + std::string(dlerror()));
    }
#endif

    handle = nullptr;
}
void SharedLibrary::addSearchPath(const std::string& path) {
    searchPaths.push_back(path);
}
