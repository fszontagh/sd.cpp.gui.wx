#ifndef _SDGUI_LIBS_SHARED_LIBRARY_H
#define _SDGUI_LIBS_SHARED_LIBRARY_H
#ifdef _WIN32
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

class SharedLibrary {
public:
    explicit SharedLibrary(const std::string& libraryPath);
    ~SharedLibrary();

    void load();
    void unload();

    template <typename T>
    T getFunction(const std::string& functionName) {
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

inline std::string getLibraryPath() const { return libraryPath; }
private:
    std::string libraryPath;
    void* handle;
};

#endif  // _SDGUI_LIBS_SHARED_LIBRARY_H
