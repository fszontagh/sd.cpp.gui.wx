#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();


    bool loadLibrary();


private:
    SharedLibrary sd_dll;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager;
    std::string tempPath;
    std::string error;
};

#endif  // EXTPROCESS_APPLICATIONLOGIC_H