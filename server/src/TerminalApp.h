#ifndef _SERVER_TERMINALAPP_H
#define _SERVER_TERMINALAPP_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/app.h>
#include <wx/event.h>
#include <wx/evtloop.h>
#include <wx/log.h>
#include <wx/textfile.h>
#include <iostream>

#include "ServerConfig.h"
#include "SocketApp.h"
#include "config.hpp"
#include "sockets-cpp/TcpServer.h"

wxDECLARE_APP(TerminalApp);

wxDEFINE_EVENT(wxEVT_THREAD_LOG, wxCommandEvent);

class TerminalApp : public wxAppConsole {
public:
    virtual bool OnInit() override;
    virtual int OnRun() override;
    virtual int OnExit() override;
    virtual bool IsGUI() const override { return false; }

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

private:
    std::vector<std::thread> threads;
    bool m_shouldExit = false;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager;
    SocketApp* socket                        = nullptr;
    std::FILE* logfile                       = nullptr;
    wxLogStderr* logger                      = nullptr;
    wxLog* oldLogger                         = nullptr;
    std::shared_ptr<ServerConfig> configData = nullptr;
    std::atomic<bool> eventHandlerReady      = false;
};

#endif  // _SERVER_TERMINALAPP_H