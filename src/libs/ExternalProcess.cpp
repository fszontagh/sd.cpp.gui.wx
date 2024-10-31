#include "ExternalProcess.h"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>
#include "wx/event.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

ExternalProcess::ExternalProcess(const std::string& command, const std::string& arguments, bool autoRestart)
    : command(command), arguments(arguments), autoRestart(autoRestart), running(false), restartRequested(false) {
    this->command = std::filesystem::canonical(this->command).string();
    this->sharedMemory = std::make_shared<SharedMemoryManager>(SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE, true);
}

ExternalProcess::~ExternalProcess() {
    stop();
}

void ExternalProcess::start(onMessageCallBack callback) {
    if (running) {
        return;
    }
    this->onMessage = callback;
#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&processInfo, sizeof(processInfo));

    if (CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo)) {
        running = true;
        outputThread = std::thread(&ExternalProcess::processOutput, this, callback);
        if (onStarted) {
            onStarted();
        }
    } else {
        throw std::runtime_error("Failed to start process: " + std::to_string(GetLastError()));
    }
#else
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork: " + std::string(strerror(errno)));
    } else if (pid == 0) {  // Child process
        std::cout << "Starting: " << command << " " << arguments << std::endl;
        execl(command.c_str(), command.c_str(), arguments.c_str(), (char*)0);
        std::cerr << "Failed to start process: " << command << std::endl;
        _exit(1);  // Exit if execl fails
    } else {       // Parent process
        processId = pid;
        running = true;
        outputThread = std::thread(&ExternalProcess::processOutput, this, callback);
        if (onStarted) {
            onStarted();
        }
    }
#endif
}

void ExternalProcess::stop() {
    if (!running)
        return;

#if defined(_WIN32) || defined(_WIN64)
    TerminateProcess(processInfo.hProcess, 0);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
#else
    kill(processId, SIGTERM);
    waitpid(processId, nullptr, 0);  // Wait for child process to terminate
#endif

    running = false;
    if (outputThread.joinable()) {
        outputThread.join();
    }
    if (onExit) {
        onExit();
    }
}

void ExternalProcess::send(std::string message) {
    sharedMemory->write(message.c_str(), message.size());
}

bool ExternalProcess::isRunning() const {
    return running;
}

void ExternalProcess::processOutput(onMessageCallBack callback) {
    while (running) {
#if defined(_WIN32) || defined(_WIN64)
        DWORD exitCode;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);
        if (exitCode != STILL_ACTIVE) {
            if (onExit) {
                autoRestart = onExit();
            }
            if (autoRestart) {
                restartRequested = true;  // Set flag to request restart
            } else {
                running = false;
            }
            return;
        }
#else
        int status;
        if (waitpid(processId, &status, WNOHANG) > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                if (onExit) {
                    autoRestart = onExit();
                }
                if (autoRestart) {
                    restartRequested = true;  // Set flag to request restart
                } else {
                    running = false;
                }
                return;
            }
        }
#endif

        char buffer[SHARED_MEMORY_SIZE];
        if (sharedMemory->read(buffer, sizeof(buffer)) && callback) {
            if (std::strlen(buffer) > 0) {
                std::string message = std::string(buffer, SHARED_MEMORY_SIZE);
                if (callback(message) == true) {
                    sharedMemory->clear();
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
    }
}

bool ExternalProcess::restartIfNeeded() {
    if (restartRequested) {
        restartRequested = false;
        restartProcess(this->onMessage);
        return true;
    }
    return false;
}

void ExternalProcess::restartProcess(onMessageCallBack callback) {
    stop();
    start(callback);
}

void ExternalProcess::setOnExitCallback(onExitCallBack callback) {
    onExit = callback;
}

void ExternalProcess::setOnStartedCallback(onStartedCallBack callback) {
    onStarted = callback;
}
