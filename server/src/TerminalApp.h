#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "libs/SnowFlakeIdGenerarot.hpp"

wxDECLARE_APP(TerminalApp);

class TerminalApp : public wxAppConsole {
public:
    virtual bool OnInit() override;
    virtual int OnRun() override;
    virtual int OnExit() override;
    virtual bool IsGUI() const override { return false; }
    void ProcessReceivedSocketPackages(sd_gui_utils::networks::Packet& packet);
    inline void sendLogEvent(const wxString& message, const wxLogLevel level = wxLOG_Info) {
        eventQueue.Push([message, level]() {
            switch (level) {
                case wxLOG_Info:
                    wxLogInfo(message);
                    break;
                case wxLOG_Warning:
                    wxLogWarning(message);
                    break;
                case wxLOG_Error:
                    wxLogError(message);
                    break;
                case wxLOG_Debug:
                    wxLogDebug(message);
                    break;
                default:
                    wxLogMessage(message);
                    break;
            }
        });
    }
    inline void sendDisconnectEvent(SocketApp::clientInfo client) {
        if (client.apikey.empty() || client.client_id == 0) {
            return;
        }
        eventQueue.Push([this, client]() {
            std::string clientDataFile = this->configData->GetClientDataPath() + std::to_string(client.client_id) + ".json";
            nlohmann::json json        = client;
            std::ofstream file(clientDataFile);
            file << std::setw(4) << json << std::endl;
            file.close();
        });
    }
    inline void sendOnTimerEvent(const SocketApp::clientInfo& client) {
        if (client.client_id == 0) {
            return;
        }
        eventQueue.Push([this, client]() {
            std::string clientDataFile = this->configData->GetClientDataPath() + std::to_string(client.client_id) + ".json";
            nlohmann::json json        = client;
            std::ofstream file(clientDataFile);
            file << std::setw(4) << json << std::endl;
            file.close();
        });
    }
    inline void JobQueueThread() {
        while (this->queueNeedToRun.load()) {
            bool running = false;
            for (const auto& job : this->queueManager->GetJobList()) {
                if (job.status & QueueStatusFlags::RUNNING_FLAG) {
                    running = true;
                    std::cout << "We have a running job: " << job.id << std::endl;
                    break;
                }
            }
            if (!running) {
                for (const auto& job : this->queueManager->GetJobList()) {
                    if (job.status == QueueStatus::PENDING) {
                        const nlohmann::json j = job;
                        char* data             = new char[j.dump().length() + 1];
                        strcpy(data, j.dump().c_str());
                        this->sharedMemoryManager->write(data, j.dump().length() + 1);
                        this->sendLogEvent(wxString::Format("Job started: %llu", job.id));
                        std::cout << "Job started: " << job.id << std::endl;
                        break;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    inline SocketApp::clientInfo ReReadClientInfo(uint64_t client_id, bool delete_old_file = false) {
        std::string clientDataFile = this->configData->GetClientDataPath() + std::to_string(client_id) + ".json";
        if (std::filesystem::exists(clientDataFile) == false) {
            return SocketApp::clientInfo();
        }
        std::ifstream file(clientDataFile);
        nlohmann::json json;
        file >> json;
        file.close();
        if (delete_old_file) {
            std::filesystem::remove(clientDataFile);
        }
        return json;
    }
    std::shared_ptr<ServerConfig> configData = nullptr;

private:
    std::atomic<bool> queueNeedToRun{true};
    void ExternalProcessRunner();
    void ProcessOutputThread();
    void ProcessLogQueue();
    bool ProcessEventHandler(std::string message);
    bool LoadModelFiles();
    void CalcModelHashes();
    void AddModelFile(const wxFileName& filename, const std::string& rootpath, const sd_gui_utils::DirTypes type, size_t& used_sizes, int& file_count, wxULongLong& used_model_sizes);
    inline static void FileHashCallBack(size_t size, std::string name, void* data) {
        auto instance = ((TerminalApp*)data);
        size          = size == 0 ? 1 : size;
        int progress  = static_cast<int>(100.0 * size / instance->currentHashingItem->size);

        if (size % (1024 * 1024) == 0) {
            std::cout << "\33[2K\r" << std::flush;  // from win10 console
            wxULongLong currentTotal    = instance->hashingProcessed.load() + size;
            wxULongLong currentRequired = instance->hashingFullSize.load();
            auto total_progress         = wxULongLong(100) * currentTotal / currentRequired;
            auto msg                    = wxString::Format(
                "[%s - %s]: %d%% (%llu/%llu) Total: %s%%",
                instance->currentHashingItem->name,
                instance->currentHashingItem->size_f,
                progress,
                size,
                instance->currentHashingItem->size,
                total_progress.ToString());

            std::cout << "\r" << msg.ToStdString() << std::flush;
        }
    }
    inline sd_gui_utils::RemoteModelInfo* GetModelByHash(const std::string sha2560) {
        for (auto& model : this->modelFiles) {
            if (model.second.sha256 == sha2560) {
                return &model.second;
            }
        }
        return nullptr;
    }

    inline std::string GetModelPathByHash(const std::string sha256) {
        for (auto& model : this->modelFiles) {
            if (model.second.sha256 == sha256) {
                return model.first;
            }
        }
        return "";
    }
    std::thread logThread;
    std::atomic<bool> eventHanlderExit{false};
    std::shared_ptr<sd_gui_utils::RemoteModelInfo> currentHashingItem = nullptr;
    std::atomic<wxULongLong> hashingFullSize{0};
    std::atomic<wxULongLong> hashingProcessed{0};

    EventQueue eventQueue;
    std::shared_ptr<SimpleQueueManager> queueManager                       = nullptr;
    std::shared_ptr<sd_gui_utils::SnowflakeIDGenerator> snowflakeGenerator = nullptr;
    std::vector<std::thread> threads;
    bool m_shouldExit                                        = false;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager = nullptr;
    std::shared_ptr<SharedLibrary> sharedLibrary             = nullptr;
    SocketApp* socket                                        = nullptr;
    std::FILE* logfile                                       = nullptr;
    wxLogStderr* logger                                      = nullptr;
    wxLog* oldLogger                                         = nullptr;
    struct subprocess_s* subprocess                          = nullptr;
    wxString extprocessCommand                               = "";
    wxArrayString extProcessParams;
    std::atomic<bool> eventHandlerReady{false};
    std::atomic<bool> extProcessNeedToRun{false};
    std::atomic<bool> extprocessIsRunning{false};
    wxTimer timer;
    std::unordered_map<std::string, sd_gui_utils::networks::RemoteModelInfo> modelFiles;
};

#endif  // _SERVER_TERMINALAPP_H