#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

#include <iostream>
#include <stdexcept>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
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
    T getFunction(const std::string& functionName);

private:
    std::string libraryPath;
    void* handle;
};

#endif  // SHARED_LIBRARY_H
