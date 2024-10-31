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
    std::string libname = libraryPath + ".dll";
    handle              = LoadLibraryA(libname.c_str());
    if (!handle) {
        throw std::runtime_error("Failed to load library: " + libraryPath);
    }
#else
    std::string libname = libraryPath + ".so";
    handle              = dlopen(libname.c_str(), RTLD_LAZY);
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

