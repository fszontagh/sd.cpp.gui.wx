#ifndef SHAREDMEMORYMANAGER_H
#define SHAREDMEMORYMANAGER_H

#include <cstring>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <mutex>

class SharedMemoryManager {
public:
    SharedMemoryManager(const std::string& shmName, size_t size, bool isMaster);
    ~SharedMemoryManager();

    bool write(const void* data, size_t size);
    bool read(void* buffer, size_t size);
    void clear();
    constexpr size_t getSize() const { return shmSize; }

private:
    std::string shmName;
    size_t shmSize = 0;
    bool isMaster  = false;
    std::mutex mutex;

#ifdef _WIN32
    HANDLE shmHandle;
    void* shmPtr;
#else
    int shmFd;
    void* shmPtr;
#endif
};

#endif  // SHAREDMEMORYMANAGER_H
