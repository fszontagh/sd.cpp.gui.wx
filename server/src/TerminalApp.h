#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"

wxDECLARE_APP(TerminalApp);

class TerminalApp : public wxAppConsole {
public:
    virtual bool OnInit() override;
    virtual int OnRun() override;
    virtual int OnExit() override;
    virtual bool IsGUI() const override { return false; }
    void ProcessReceivedSocketPackages(const sd_gui_utils::networks::Packet& packet);
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
    std::shared_ptr<ServerConfig> configData = nullptr;

private:
    void ExternalProcessRunner();
    void ProcessOutputThread();
    void ProcessLogQueue();
    // process the messages from the shm
    bool ProcessEventHandler(std::string message);
    bool LoadModelFiles();
    void CalcModelHashes();
    inline static void FileHashCallBack(size_t size, std::string name, void* data) {
        auto instance = ((TerminalApp*)data);
        size          = size == 0 ? 1 : size;
        int progress  = static_cast<int>(100.0 * size / instance->currentHashingItem->size);

        if (size % (1024 * 1024) == 0) {
            wxULongLong currentTotal    = instance->hashingProcessed.load() + size;
            wxULongLong currentRequired = instance->hashingFullSize.load();
            auto total_progress         = wxULongLong(100) * currentTotal / currentRequired;
            auto msg                    = wxString::Format(
                "[%s]: %d%% (%llu/%llu) Total: %s%%\t",
                instance->currentHashingItem->name, progress, size, instance->currentHashingItem->size, total_progress.ToString());

            std::cout << "\r" << msg.ToStdString() << std::flush;
        }
    }
    std::thread logThread;
    std::atomic<bool> eventHanlderExit{false};
    sd_gui_utils::RemoteModelInfo* currentHashingItem = nullptr;
    std::atomic<wxULongLong> hashingFullSize{0};
    std::atomic<wxULongLong> hashingProcessed{0};

    EventQueue eventQueue;
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