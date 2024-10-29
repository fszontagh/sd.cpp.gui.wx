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

#if defined(_WIN32) || defined(_WIN64)
    handle = LoadLibraryA(libraryPath.c_str());
    if (!handle) {
        throw std::runtime_error("Failed to load library: " + libraryPath);
    }
#else
    handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("Failed to load library: " + std::string(dlerror()));
    }
#endif
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

template <typename T>
T SharedLibrary::getFunction(const std::string& functionName) {
    if (!handle) {
        throw std::runtime_error("Library is not loaded. Cannot get function: " + functionName);
    }

#if defined(_WIN32) || defined(_WIN64)
    FARPROC func = GetProcAddress(static_cast<HMODULE>(handle), functionName.c_str());
    if (!func) {
        throw std::runtime_error("Failed to get function: " + functionName);
    }
    return reinterpret_cast<T>(func);
#else
    void* func = dlsym(handle, functionName.c_str());
    if (!func) {
        throw std::runtime_error("Failed to get function: " + std::string(dlerror()));
    }
    return reinterpret_cast<T>(func);
#endif
}
