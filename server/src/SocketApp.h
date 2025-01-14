#ifndef _SERVER_SOCKETAPP_H
#define _SERVER_SOCKETAPP_H

#include "sockets-cpp/TcpServer.h"

inline auto LogPrinter = [](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl; };

class TerminalApp;

class SocketApp {
public:
    struct clientInfo {
        std::string host;
        uint16_t port;
        int idx;
        long connected_at;
        std::string apikey;
        long last_keepalive = 0;
        uint64_t client_id  = 0;
        uint64_t tx         = 0;
        uint64_t rx         = 0;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(clientInfo, host, port, idx, connected_at, apikey, last_keepalive, client_id, tx, rx)
        inline void copyFrom(const SocketApp::clientInfo& other) {
            this->client_id = other.client_id;
            this->tx += other.tx;
            this->rx += other.rx;
        }
    };

    explicit SocketApp(const char* listenAddr, uint16_t port, TerminalApp* parent = nullptr);

    virtual ~SocketApp();

    void onClientConnect(const sockets::ClientHandle& client);

    void onReceiveClientData(const sockets::ClientHandle& client, const char* data, size_t size);

    void onClientDisconnect(const sockets::ClientHandle& client, const sockets::SocketRet& ret);

    void sendMsg(int idx, const char* data, size_t len);
    void sendMsg(int idx, sd_gui_utils::networks::Packet& packet);
    void DisconnectClient(int idx);
    void parseMsg(sd_gui_utils::networks::Packet& packet);
    inline bool isRunning() { return this->needToRun.load() == true; }
    inline void stop() { this->needToRun.store(false); }
    void OnTimer();
    const SocketApp::clientInfo GetClientInfo(int idx) {
        if (this->m_clientInfo.contains(idx)) {
            return this->m_clientInfo.at(idx);
        }
        return SocketApp::clientInfo();
    }

private:
    sockets::SocketOpt m_socketOpt;
    sockets::TcpServer<SocketApp> m_server;
    int m_clientIdx = 0;
    std::map<sockets::ClientHandle, clientInfo> m_clientInfo;
    std::mutex m_mutex;
    std::atomic<bool> needToRun{true};
    TerminalApp* parent = nullptr;
    std::vector<char> buffer;
    size_t expected_size = 0;
};

#endif  // _SERVER_SOCKETAPP_H