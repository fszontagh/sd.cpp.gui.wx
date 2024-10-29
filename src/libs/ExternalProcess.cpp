#include "ExternalProcess.h"

ExternalProcess::ExternalProcess(const std::string& command, bool autoRestart)
    : command(command), autoRestart(autoRestart), running(false) {
#if defined(_WIN32) || defined(_WIN64)
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
#else
    pipe(pipeRead);
    pipe(pipeWrite);
#endif
}

ExternalProcess::~ExternalProcess() {
    stop();
}

void ExternalProcess::start() {
    if (running) return;

#if defined(_WIN32) || defined(_WIN64)
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&processInfo, sizeof(processInfo));

    if (CreateProcessA(NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &processInfo)) {
        running = true;
        outputThread = std::thread(&ExternalProcess::processOutput, this);
    }
#else
    if ((processId = fork()) == 0) {
        // Child process
        dup2(pipeWrite[1], STDOUT_FILENO);
        dup2(pipeWrite[1], STDERR_FILENO);
        close(pipeRead[0]);
        close(pipeWrite[0]);

        execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
        exit(1);
    } else {
        running = true;
        outputThread = std::thread(&ExternalProcess::processOutput, this);
    }
#endif
}

void ExternalProcess::stop() {
    if (!running) return;

#if defined(_WIN32) || defined(_WIN64)
    TerminateProcess(processInfo.hProcess, 0);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);
#else
    kill(processId, SIGTERM);
    close(pipeRead[0]);
    close(pipeWrite[1]);
#endif

    running = false;
    if (outputThread.joinable()) {
        outputThread.join();
    }
}

void ExternalProcess::send(const std::string& message) {
    messageHandler.send(message);
#if defined(_WIN32) || defined(_WIN64)
    DWORD bytesWritten;
    WriteFile(hWritePipe, message.c_str(), message.size(), &bytesWritten, NULL);
#else
    write(pipeWrite[1], message.c_str(), message.size());
#endif
}

std::string ExternalProcess::receive() {
    return messageHandler.receive();
}

bool ExternalProcess::isRunning() const {
    return running;
}

void ExternalProcess::processOutput() {
    while (running) {
        std::string output = receive();
        if (!output.empty()) {
            std::cout << "Output: " << output << std::endl;
        }

        // Check if the process has stopped
#if defined(_WIN32) || defined(_WIN64)
        DWORD exitCode;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);
        if (exitCode != STILL_ACTIVE && autoRestart) {
            restartProcess();
        }
#else
        int status;
        if (waitpid(processId, &status, WNOHANG) > 0) {
            if (WIFEXITED(status) && autoRestart) {
                restartProcess();
            }
        }
#endif
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ExternalProcess::restartProcess() {
    stop();
    start();
}
