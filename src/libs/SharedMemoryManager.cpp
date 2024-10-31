#include "SharedMemoryManager.h"
#include <iostream>
#include <stdexcept>

SharedMemoryManager::SharedMemoryManager(const std::string& shmName, size_t size, bool isMaster)
    : shmName(shmName), shmSize(size), isMaster(isMaster) {
#ifdef _WIN32
    shmHandle = NULL;
    shmPtr    = nullptr;

    if (isMaster) {
        shmHandle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, static_cast<DWORD>(shmSize), shmName.c_str());
        if (shmHandle == NULL)
            throw std::runtime_error("Failed to create shared memory.");
    } else {
        shmHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, shmName.c_str());
        if (shmHandle == NULL)
            throw std::runtime_error("Failed to open shared memory.");
    }
    shmPtr = MapViewOfFile(shmHandle, FILE_MAP_ALL_ACCESS, 0, 0, shmSize);
    if (shmPtr == nullptr)
        throw std::runtime_error("Failed to map shared memory.");

#else
    if (isMaster) {
        shmFd = shm_open(shmName.c_str(), O_CREAT | O_RDWR, 0666);
        if (shmFd == -1)
            throw std::runtime_error("Failed to create shared memory.");
        if (ftruncate(shmFd, shmSize) == -1)
            throw std::runtime_error("Failed to set size for shared memory.");
    } else {
        shmFd = shm_open(shmName.c_str(), O_RDWR, 0666);
        if (shmFd == -1)
            throw std::runtime_error("Failed to open shared memory.");
    }
    shmPtr = mmap(0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (shmPtr == MAP_FAILED)
        throw std::runtime_error("Failed to map shared memory.");
#endif

    if (isMaster) {
        this->clear();
    }
}

SharedMemoryManager::~SharedMemoryManager() {
#ifdef _WIN32
    if (shmPtr)
        UnmapViewOfFile(shmPtr);
    if (shmHandle)
        CloseHandle(shmHandle);
    if (isMaster) {
        if (!DeleteFileA(shmName.c_str()))
            std::cerr << "Failed to delete shared memory." << std::endl;
    }
#else
    if (shmPtr)
        munmap(shmPtr, shmSize);
    if (shmFd != -1)
        close(shmFd);
    if (isMaster)
        shm_unlink(shmName.c_str());
#endif
}

bool SharedMemoryManager::write(const void* data, size_t size) {
    if (size > shmSize) {
        return false;
    }
    // clear before write
    this->clear();
    std::memcpy(shmPtr, data, size);
    return true;
}

bool SharedMemoryManager::read(void* buffer, size_t size) {
    if (size > shmSize) {
        return false;
    }
    std::memcpy(buffer, shmPtr, size);
    return true;
}

void SharedMemoryManager::clear() {
    std::memset(shmPtr, 0, shmSize);
}
