#ifndef _SERVER_SOCKETAPP_H
#define _SERVER_SOCKETAPP_H

#include "sockets-cpp/TcpServer.h"

class TerminalApp;

class SocketApp {
public:
    struct clientInfo {
        std::string host;
        uint16_t port;
        int idx;
        long connected_at;
        long disconnected_at = 0;
        std::string apikey;
        long last_keepalive                    = 0;
        uint64_t client_id                     = 0;
        uint64_t tx                            = 0;
        uint64_t rx                            = 0;
        std::string version                    = {};
        std::string application_system_name    = {};
        std::string application_arch           = {};
        std::string application_system_version = {};

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(clientInfo, host, port, idx, connected_at, disconnected_at, apikey, last_keepalive, client_id, tx, rx, version, application_system_name, application_arch, application_system_version)
        inline void copyFrom(const SocketApp::clientInfo& other) {
            this->client_id                  = other.client_id;
            this->apikey                     = other.apikey;
            this->port                       = other.port;
            this->host                       = other.host;
            this->disconnected_at            = other.disconnected_at;
            this->last_keepalive             = other.last_keepalive;
            this->version                    = other.version;
            this->application_system_name    = other.application_system_name;
            this->application_arch           = other.application_arch;
            this->application_system_version = other.application_system_version;

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
    size_t sendMsg(int idx, sd_gui_utils::networks::Packet& packet);
    void DisconnectClient(int idx);
    void parseMsg(sd_gui_utils::networks::Packet& packet);
    inline bool isRunning() { return this->needToRun.load() == true; }
    void stop();
    void OnTimer();
    int GetNextClientId();
    const SocketApp::clientInfo GetClientInfo(int idx) {
        if (this->m_clientInfo.contains(idx)) {
            return this->m_clientInfo.at(idx);
        }
        return SocketApp::clientInfo();
    }
    std::vector<SocketApp::clientInfo> GetClientsList() {
        std::vector<SocketApp::clientInfo> list;
        for (const auto client : this->m_clientInfo) {
            list.emplace_back(client.second);
        }
        return list;
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