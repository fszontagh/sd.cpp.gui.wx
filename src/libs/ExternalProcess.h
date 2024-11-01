#ifndef EXTERNAL_PROCESS_H
#define EXTERNAL_PROCESS_H

#include <sys/wait.h>
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
typedef std::function<bool(bool)> onExitCallBack;
typedef std::function<void()> onStartedCallBack;
typedef std::function<void(std::string)> onStdCallBack;

class ExternalProcess {
public:
    // Constructor to initialize with command, arguments, and auto-restart option
    ExternalProcess(const std::string& command, const std::string& arguments, bool autoRestart = false);
    ~ExternalProcess();

    void start(onMessageCallBack callback = nullptr);
    void setOnExitCallback(onExitCallBack callback);
    void setOnStartedCallback(onStartedCallBack callback);
    void setOnStdOutCallback(onStdCallBack callback);
    void setOnStdErrCallback(onStdCallBack callback);
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
    std::thread stdoutThread;
    std::thread stderrThread;
    void processOutput(onMessageCallBack callback);
    void restartProcess(onMessageCallBack callback = nullptr);
    void readPipeOutput(int pipeFd, onStdCallBack callback);

    onExitCallBack onExit       = nullptr;
    onStartedCallBack onStarted = nullptr;
    onMessageCallBack onMessage = nullptr;
    onStdCallBack onStdOut      = nullptr;
    onStdCallBack onStdErr      = nullptr;

    std::shared_ptr<SharedMemoryManager> sharedMemory;

    static std::unordered_map<pid_t, ExternalProcess*> instances;
    static std::mutex instanceMutex;

#if defined(_WIN32) || defined(_WIN64)
    PROCESS_INFORMATION processInfo;
    HANDLE sharedMemoryHandle;
#else
    pid_t processId;
    int shmFd;
#endif

    inline static void sigchldHandler(int /*signum*/) {
        int status;
        pid_t pid;

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            std::lock_guard<std::mutex> lock(instanceMutex);
            auto it = instances.find(pid);

            if (it != instances.end()) {
                ExternalProcess* instance = it->second;
                if (instance && instance->onExit) {
                    instance->onExit(false);  
                }
                instances.erase(it);
            }
        }
    }
};

#endif  // EXTERNAL_PROCESS_H
