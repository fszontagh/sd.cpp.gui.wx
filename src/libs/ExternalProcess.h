#ifndef EXTERNAL_PROCESS_H
#define EXTERNAL_PROCESS_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "MessageHandler.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

class ExternalProcess {
public:
    ExternalProcess(const std::string& command, bool autoRestart = false);
    ~ExternalProcess();

    void start();
    void stop();
    void send(const std::string& message);
    std::string receive();
    bool isRunning() const;

private:
    void processOutput();
    void restartProcess();

    std::string command;
    bool autoRestart;
    std::atomic<bool> running;
    std::thread outputThread;
    MessageHandler messageHandler;  // Üzenetkezelő példány

#if defined(_WIN32) || defined(_WIN64)
    PROCESS_INFORMATION processInfo;
    HANDLE hReadPipe;
    HANDLE hWritePipe;
#else
    pid_t processId;
    int pipeRead[2];
    int pipeWrite[2];
#endif
};

#endif  // EXTERNAL_PROCESS_H
