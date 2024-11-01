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

std::unordered_map<pid_t, ExternalProcess*> ExternalProcess::instances;
std::mutex ExternalProcess::instanceMutex;

ExternalProcess::ExternalProcess(const std::string& command, const std::string& arguments, bool autoRestart)
    : command(command), arguments(arguments), autoRestart(autoRestart), running(false), restartRequested(false) {
    this->command      = std::filesystem::canonical(this->command).string();
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
    SECURITY_ATTRIBUTES sa;
    sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle       = TRUE;
    sa.lpSecurityDescriptor = NULL;
    // Create pipes for stdout and stderr
    if (this->onStdOut) {
        HANDLE stdoutRead, stdoutWrite
                               CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0);
        SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);
    }

    if (this->onStdErr) {
        HANDLE stderrRead, stderrWrite;
        CreatePipe(&stderrRead, &stderrWrite, &sa, 0);
        SetHandleInformation(stderrRead, HANDLE_FLAG_INHERIT, 0);
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (this->onStdOut) {
        si.hStdOutput = stdoutWrite;
    }
    if (this->onStdErr) {
        si.hStdError = stderrWrite;
    }

    ZeroMemory(&processInfo, sizeof(processInfo));

    if (CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &processInfo)) {
        running = true;
        // Close unused ends of the pipes

        if (this->onStdOut) {
            CloseHandle(stdoutWrite);
            stdoutThread = std::thread(&ExternalProcess::readPipeOutput, this, stdoutRead, this->onStdOut);
        }
        if (this->onStdErr) {
            CloseHandle(stderrWrite);
            stderrThread = std::thread(&ExternalProcess::readPipeOutput, this, stderrRead, this->onStdErr);
        }
        processWatchThread = std::thread([this]() {
            WaitForSingleObject(processInfo.hProcess, INFINITE);
            DWORD exitCode;
            GetExitCodeProcess(processInfo.hProcess, &exitCode);

            if (onExit) {
                autoRestart = onExit(false);
            }

            if (autoRestart) {
                restartRequested = true;  // Újraindítás kérés beállítása
            } else {
                running = false;
            }

            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        });

        if (onStarted) {
            onStarted();
        }
        outputThread = std::thread(&ExternalProcess::processOutput, this, this->onMessage);
    } else {
        throw std::runtime_error("Failed to start process: " + std::to_string(GetLastError()));
    }
#else
    signal(SIGCHLD, sigchldHandler);
    int stdoutPipe[2], stderrPipe[2];
    if (pipe(stdoutPipe) == -1 || pipe(stderrPipe) == -1) {
        throw std::runtime_error("Failed to create pipes for stdout or stderr.");
    }

    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Failed to fork: " + std::string(strerror(errno)));
    } else if (pid == 0) {  // Child process
        close(stdoutPipe[0]);
        close(stderrPipe[0]);

        if (this->onStdOut) {
            dup2(stdoutPipe[1], STDOUT_FILENO);
        }

        if (this->onStdErr) {
            dup2(stderrPipe[1], STDERR_FILENO);
        }

        execl(command.c_str(), command.c_str(), arguments.c_str(), (char*)0);
        _exit(1);  // Exit if execl fails
    } else {       // Parent process
        processId = pid;
        running   = true;

        if (this->onStdOut) {
            close(stdoutPipe[1]);
            stdoutThread = std::thread(&ExternalProcess::readPipeOutput, this, stdoutPipe[0], this->onStdOut);
        }
        if (this->onStdErr) {
            close(stderrPipe[1]);
            stderrThread = std::thread(&ExternalProcess::readPipeOutput, this, stderrPipe[0], this->onStdErr);
        }

        if (onStarted) {
            onStarted();
        }
        outputThread = std::thread(&ExternalProcess::processOutput, this, this->onMessage);
        std::lock_guard<std::mutex> lock(instanceMutex);
        instances[processId] = this;
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
    {
        std::lock_guard<std::mutex> lock(instanceMutex);
        instances.erase(processId);
    }
#endif

    running = false;
    if (stdoutThread.joinable()) {
        stdoutThread.join();
    }

    if (stderrThread.joinable()) {
        stderrThread.join();
    }
    if (outputThread.joinable()) {
        outputThread.join();
    }
    if (onExit) {
        onExit(true);
    }
}

void ExternalProcess::readPipeOutput(int pipeFd, onStdCallBack callback) {
    char buffer[EPROCESS_STD_BUFFER];
    while (running) {
#if defined(_WIN32) || defined(_WIN64)
        DWORD bytesRead;
        if (ReadFile((HANDLE)pipeFd, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            if (callback)
                callback(std::string(buffer));
        }
#else
        ssize_t bytesRead = read(pipeFd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            if (callback)
                callback(std::string(buffer));
        }
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
            running = false;
            if (onExit) {
                autoRestart = onExit(false);
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
                running = false;
                if (onExit) {
                    autoRestart = onExit(false);
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

/**
 * @brief Restart the process if the flag is set.
 *
 * This function restarts the process if the restart flag is set.
 * The flag is set when the process exits and the onExit callback
 * returns true.
 *
 * @return true if the process was restarted, false otherwise.
 */
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

void ExternalProcess::setOnStdErrCallback(onStdCallBack callback) {
    onStdErr = callback;
}

void ExternalProcess::setOnStdOutCallback(onStdCallBack callback) {
    onStdOut = callback;
}
