#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

#include "libs/SharedLibrary.h"

#include <wx/app.h>
#include <wx/evtloop.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/timer.h>

#include <iostream>
#include "libs/json.hpp"
#include "network/RemoteModelInfo.h"

#include "ServerConfig.h"
#include "SocketApp.h"
#include "helpers/sslUtils.hpp"
#include "libs/ExternalProcess.h"

#include "EventQueue.h"

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
    std::thread logThread;
    std::atomic<bool> eventHanlderExit{false};

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