#ifndef EXTERNALPROCESSHELPER_H
#define EXTERNALPROCESSHELPER_H

#include <queue>
class ExternalProcessHelper {
private:
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;

public:
    enum class ProcessType { none,
                             diffuser,
                             llama };

    std::function<void()> onStart;
    std::function<void(const char*, size_t)> onStdOut;
    std::function<void(const char*, size_t)> onStdErr;
    std::function<void()> onExit;
    std::function<bool(const char*, size_t)> onShmMessage;

    ExternalProcessHelper(wxString processName, ProcessType type, wxArrayString params, wxString shmName, unsigned long shmSize)
        : processType(type), extProcessArguments(params), shmSize(shmSize), shmName(shmName) {
        this->extProcessCommand = findBinary(processName, type);
        if (this->extProcessCommand.IsEmpty()) {
            this->error = "Failed to find the external process binary";
            throw std::runtime_error(this->error.ToStdString());
            return;
        }

        storedArgs.emplace_back(this->extProcessCommand.ToStdString());
        for (const auto& p : params) {
            this->storedArgs.emplace_back(p.ToStdString());
        }

        this->sharedMemory         = std::make_shared<SharedMemoryManager>(shmName.ToStdString(), shmSize, true);
        this->queueThreadNeedToRun = true;
        this->processQueueThread   = std::thread(&ExternalProcessHelper::ProcessQueueWorker, this);
    }

    ~ExternalProcessHelper() {
        std::lock_guard<std::mutex> lock(this->processMutex);

        this->extProcessNeedToRun  = false;
        this->queueThreadNeedToRun = false;

        if (this->processMonitorThread.joinable()) {
            this->processMonitorThread.join();
        }
        if (this->processOutputsThread.joinable()) {
            this->processOutputsThread.join();
        }
        if (this->sharedMemoryThread.joinable()) {
            this->sharedMemoryThread.join();
        }
        if (this->processQueueThread.joinable()) {
            this->processQueueThread.join();
        }

        if (this->sharedMemory) {
            std::string exit_command = "exit";
            this->sharedMemory->write(exit_command.c_str(), exit_command.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (this->subprocess) {
            int state = subprocess_terminate(this->subprocess);
            std::cout << "Terminatin process... " << std::endl;
            while (state != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                state = subprocess_terminate(this->subprocess);
            }
            delete this->subprocess;
            this->subprocess = nullptr;
        }
    }

    bool Start() {
        std::lock_guard<std::mutex> lock(this->processMutex);
        if (this->IsAlive()) {
            return false;
        }
        this->subprocess = new subprocess_s();

        this->command_line.clear();
        for (auto& arg : this->storedArgs) {
            this->command_line.push_back(arg.c_str());
        }

        command_line.push_back(nullptr);

        std::cout << "Command line arguments:" << std::endl;
        for (const auto& arg : this->command_line) {
            if (arg != nullptr) {
                std::cout << arg << std::endl;
            }
        }

        if (subprocess_create(this->command_line.data(), this->subprocessOptions, this->subprocess) != 0) {
            this->error = "Failed to create the subprocess";
            return false;
        }
        this->extProcessNeedToRun = true;

        this->processMonitorThread = std::thread(&ExternalProcessHelper::ProcessMonitorLoop, this);
        this->processOutputsThread = std::thread(&ExternalProcessHelper::HandleProcessOutputs, this);
        this->sharedMemoryThread   = std::thread(&ExternalProcessHelper::SharedMemoryThread, this);

        if (this->onStart) {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(this->onStart);
        }

        return true;
    }
    inline const wxString GetFullCommand() const {
        wxString cmd;
        for (const auto& arg : this->storedArgs) {
            cmd += arg + " ";
        }
        return cmd;
    }
    bool Stop(bool is_restart = false) {
        std::lock_guard<std::mutex> lock(this->processMutex);
        if (!this->subprocess) {
            return false;
        }
        this->extProcessNeedToRun = false;
        if (this->processMonitorThread.joinable()) {
            this->processMonitorThread.join();
        }
        if (this->processOutputsThread.joinable()) {
            this->processOutputsThread.join();
        }
        if (this->sharedMemoryThread.joinable()) {
            this->sharedMemoryThread.join();
        }

        if (!is_restart && this->onExit) {
            std::cout << "Called onExit white on stop and not restart" << std::endl;
            // push to the queue
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push(this->onExit);
            }
        }

        if (this->IsAlive()) {
            subprocess_terminate(this->subprocess);
        }
        delete this->subprocess;
        this->subprocess = nullptr;
        return true;
    }

    bool Restart() {
        this->Stop(true);
        return this->Start();
    }
    bool IsAlive() {
        if (this->subprocess) {
            return subprocess_alive(this->subprocess) != 0;
        }
        return false;
    }
    wxString GetError() const { return this->error; }
    ExternalProcessHelper::ProcessType GetProcessType() const { return this->processType; }

    inline wxString read() {
        wxString msg = wxEmptyString;
        std::unique_ptr<char[]> buffer(new char[this->shmSize]);
        if (this->sharedMemory->read(buffer.get(), this->shmSize)) {
            msg = wxString(buffer.get());
        }
        return msg;
    }

    inline void write(const wxString& msg) {
        size_t size = std::min(msg.size(), this->shmSize);
        this->sharedMemory->write(msg.ToStdString().c_str(), size);
    }

    inline void clean() {
        this->sharedMemory->clear();
    }

    inline static const wxString buildDllPathFromName(wxString dllName) {
        wxString dllFullName = wxEmptyString;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        wxFileName dllFullPath = wxFileName(wxStandardPaths::Get().GetExecutablePath());
        dllFullPath.SetName(dllName);
        dllFullPath.SetExt("dll");
        dllFullName = dllFullPath.GetFullPath();
#else
        dllFullName.Append(dllName);
        dllFullName.Append(".so");
#endif
        return dllFullName;
    }

private:
    wxString findBinary(const wxString& binary_name, ExternalProcessHelper::ProcessType processType) {
        wxString path;
        wxString currentPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
        wxFileName f(currentPath, binary_name);
        if (!f.Exists()) {
            f.AppendDir(processType == ExternalProcessHelper::ProcessType::llama ? "llama" : "extprocess");
            f.SetName(binary_name);
        }

        if (!f.Exists()) {
            f.AppendDir(isDEBUG ? "Debug" : "Release");
            f.SetName(binary_name);
        }
        if (f.Exists()) {
            path = f.GetFullPath();
        }
        return path;
    }

    void SharedMemoryThread() {
        while (this->extProcessNeedToRun) {
            std::unique_ptr<char[]> buffer(new char[this->shmSize]);

            this->sharedMemory->read(buffer.get(), SHARED_MEMORY_SIZE);
            size_t size = std::strlen(buffer.get());
            if (size > 0 && this->onShmMessage) {
                auto const state = this->onShmMessage(buffer.get(), size);
                if (state == true) {
                    this->sharedMemory->clear();
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    void HandleProcessOutputs() {
        while (this->extProcessNeedToRun) {
            if (this->IsAlive()) {
                char stdoutBuffer[1024] = {0};
                char stderrBuffer[1024] = {0};
                unsigned int size       = sizeof(stdoutBuffer);
                unsigned int stdoutRead = subprocess_read_stdout(this->subprocess, stdoutBuffer, size);
                unsigned int stderrRead = subprocess_read_stderr(this->subprocess, stderrBuffer, size);
                if (stdoutRead > 0 && this->onStdOut) {
                    this->onStdOut(stdoutBuffer, stdoutRead);
                }
                if (stderrRead > 0 && this->onStdErr) {
                    this->onStdErr(stderrBuffer, stderrRead);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void ProcessMonitorLoop() {
        while (this->extProcessNeedToRun) {
            if (!this->IsAlive()) {
                std::cout << "Task died... " << std::endl;
                {
                    if (this->onExit) {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        taskQueue.push(this->onExit);
                    }
                }

                this->extProcessNeedToRun = false;
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    void ProcessQueueWorker() {
        while (this->queueThreadNeedToRun) {
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!taskQueue.empty()) {
                    task = std::move(taskQueue.front());
                    taskQueue.pop();
                }
            }
            if (task) {
                task();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::atomic<bool> extProcessNeedToRun             = {false};
    std::atomic<bool> queueThreadNeedToRun            = {true};
    std::shared_ptr<SharedMemoryManager> sharedMemory = nullptr;
    const int subprocessOptions                       = subprocess_option_no_window | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment;
    ProcessType processType                           = ProcessType::none;
    wxString extProcessCommand                        = wxEmptyString;
    wxArrayString extProcessArguments;
    wxString error                  = wxEmptyString;
    size_t shmSize                  = 0;
    wxString shmName                = wxEmptyString;
    struct subprocess_s* subprocess = nullptr;
    std::vector<const char*> command_line;
    std::thread processMonitorThread = {};
    std::thread processOutputsThread = {};
    std::thread sharedMemoryThread   = {};
    std::thread processQueueThread   = {};
    std::mutex processMutex;
    std::vector<std::string> storedArgs;
};

#endif  // EXTERNALPROCESSHELPER_H
