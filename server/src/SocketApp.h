#ifndef _SERVER_SOCKETAPP_H
#define _SERVER_SOCKETAPP_H
#include <map>

#include <wx/log.h>
#include <wx/time.h>
#include <wx/timer.h>
#include "libs/json.hpp"

#include "network/packets.h"
#include "sockets-cpp/TcpServer.h"



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
    void sendMsg(int idx, const sd_gui_utils::networks::Packet& packet);
    void DisconnectClient(int idx);
    void parseMsg(sd_gui_utils::networks::Packet& packet);
    inline bool isRunning() { return this->needToRun.load() == true; }
    inline void stop() { this->needToRun.store(false); }
    void OnTimer();

private:
    struct clientInfo {
        std::string host;
        uint16_t port;
        int idx;
        long connected_at;
        std::string apikey;
        long last_keepalive = 0;
    };
    sockets::SocketOpt m_socketOpt;
    sockets::TcpServer<SocketApp> m_server;
    int m_clientIdx = 0;
    std::map<sockets::ClientHandle, clientInfo> m_clientInfo;
    std::mutex m_mutex;
    std::atomic<bool> needToRun{true};
    TerminalApp* parent         = nullptr;
    std::vector<char> buffer;
    size_t expected_size = 0;
};

#endif  // _SERVER_SOCKETAPP_H