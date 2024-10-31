#ifndef EXTERNAL_PROCESS_H
#define EXTERNAL_PROCESS_H

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include "SharedMemoryManager.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/types.h>
#endif
#include "extprocess/config.hpp"

typedef std::function<bool(std::string)> onMessageCallBack;
typedef std::function<bool()> onExitCallBack;
typedef std::function<void()> onStartedCallBack;

class ExternalProcess {
public:
    // Constructor to initialize with command, arguments, and auto-restart option
    ExternalProcess(const std::string& command, const std::string& arguments, bool autoRestart = false);
    ~ExternalProcess();

    void start(onMessageCallBack callback = nullptr);
    void setOnExitCallback(onExitCallBack callback);
    void setOnStartedCallback(onStartedCallBack callback);
    void stop();
    void send(std::string data);
    void clear();
    std::string receive();
    bool isRunning() const;
    inline std::string getArguments() const { return arguments; }
    bool restartIfNeeded();

private:
    std::atomic<bool> restartRequested;
    std::string command;
    std::string arguments;
    bool autoRestart = false;
    std::atomic<bool> running;
    std::thread outputThread;
    void processOutput(onMessageCallBack callback);
    void restartProcess(onMessageCallBack callback = nullptr);
    onExitCallBack onExit       = nullptr;
    onStartedCallBack onStarted = nullptr;
    onMessageCallBack onMessage = nullptr;

    std::shared_ptr<SharedMemoryManager> sharedMemory;

#if defined(_WIN32) || defined(_WIN64)
    PROCESS_INFORMATION processInfo;
    HANDLE sharedMemoryHandle;
#else
    pid_t processId;
    int shmFd;
#endif
};

#endif  // EXTERNAL_PROCESS_H
