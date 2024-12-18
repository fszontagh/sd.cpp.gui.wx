#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

#include "libs/SharedLibrary.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/app.h>
#include <wx/event.h>
#include <wx/evtloop.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <iostream>
#include "libs/json.hpp"
#include "network/RemoteModelInfo.h"

#include "ServerConfig.h"
#include "SocketApp.h"
#include "helpers/sslUtils.hpp"
#include "libs/ExternalProcess.h"

wxDECLARE_APP(TerminalApp);

wxDEFINE_EVENT(wxEVT_THREAD_LOG, wxCommandEvent);

class TerminalApp : public wxAppConsole {
public:
    virtual bool OnInit() override;
    virtual int OnRun() override;
    virtual int OnExit() override;
    virtual bool IsGUI() const override { return false; }
    void ProcessReceivedSocketPackages(const sd_gui_utils::networks::Packet& packet);
    inline void sendLogEvent(const wxString& strLogMsg, const wxLogLevel level = wxLOG_Info) {
        if (!this->eventHandlerReady) {
            std::cerr << "Event handler not ready. Dropping log: " << strLogMsg << std::endl;
            return;
        }
        wxCommandEvent evt(wxEVT_THREAD_LOG);
        evt.SetString(strLogMsg);
        evt.SetInt((int)level);
        wxTheApp->AddPendingEvent(evt);
    }
    std::shared_ptr<ServerConfig> configData = nullptr;


private:
    void ExternalProcessRunner();
    // process the messages from the shm
    bool ProcessEventHandler(std::string message);
    bool LoadModelFiles();

    std::vector<std::thread> threads;
    bool m_shouldExit                                        = false;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager = nullptr;
    std::shared_ptr<SharedLibrary> sharedLibrary             = nullptr;
    SocketApp* socket                                        = nullptr;
    std::FILE* logfile                                       = nullptr;
    wxLogStderr* logger                                      = nullptr;
    wxLog* oldLogger                                         = nullptr;
    std::atomic<bool> eventHandlerReady                      = false;
    struct subprocess_s* subprocess                          = nullptr;
    wxString extprocessCommand                               = "";
    wxString extProcessParam                                 = "";
    std::atomic<bool> extprocessIsRunning                    = false;
    std::atomic<bool> extProcessNeedToRun                    = true;
    wxTimer timer;
    std::unordered_map<std::string, sd_gui_utils::networks::RemoteModelInfo> modelFiles;
};

#endif  // _SERVER_TERMINALAPP_H