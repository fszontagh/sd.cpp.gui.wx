#ifndef _SERVER_SOCKETAPP_H
#define _SERVER_SOCKETAPP_H

#include <wx/log.h>
#include <wx/time.h>


#include "sockets-cpp/TcpServer.h"

#include "libs/json.hpp"

inline auto LogPrinter = [](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl; };

class TerminalApp;

class SocketApp {
public:

    explicit SocketApp(const char* listenAddr, uint16_t port, TerminalApp* parent = nullptr);

    virtual ~SocketApp();

    void onClientConnect(const sockets::ClientHandle& client);

    void onReceiveClientData(const sockets::ClientHandle& client, const char* data, size_t size);

    void onClientDisconnect(const sockets::ClientHandle& client, const sockets::SocketRet& ret);

    void sendMsg(int idx, const char* data, size_t len);
    inline bool isRunning() { return this->needToRun == true; }
    inline void stop() { this->needToRun = false; }

private:
    struct clientInfo {
        std::string host;
        uint16_t port;
        int idx;
        long connected_at;
    };
    sockets::SocketOpt m_socketOpt;
    sockets::TcpServer<SocketApp> m_server;
    int m_clientIdx = 0;
    std::map<int, clientInfo> m_clientInfo;
    std::mutex m_mutex;
    std::atomic<bool> needToRun = true;
    TerminalApp* parent = nullptr;
};

#endif // _SERVER_SOCKETAPP_H