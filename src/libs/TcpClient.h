#ifndef _LIBS_TCPCLIENT_H_
#define _LIBS_TCPCLIENT_H_

#include "../network/packets.h"

namespace sd_gui_utils {
    inline namespace networks {
        using TcpClientOnMessage     = std::function<void(int)>;
        using TcpClientOnConnect     = std::function<void()>;
        using TcpClientOntDisconnect = std::function<void(std::string)>;
        using TcpClientOnError       = std::function<void(const std::string&)>;

        class TcpClient {
            sockets::TcpClient<TcpClient> m_client;
            void sendMsg(const sd_gui_utils::networks::Packet& packet);
            std::unordered_map<size_t, sd_gui_utils::networks::Packet> receivedPackets;
            std::vector<char> buffer;
            size_t expected_size = 0;
            void HandlePackets(sd_gui_utils::networks::Packet& packet);
            std::atomic<bool> connected{false};
            std::string host, disconnect_reason;
            int port;

        public:
            TcpClientOnMessage onMessageClb        = nullptr;
            TcpClientOnConnect onConnectClb        = nullptr;
            TcpClientOntDisconnect onDisconnectClb = nullptr;
            TcpClientOnError onErrorClb            = nullptr;
            TcpClient()
                : m_client(*this) {};

            TcpClient(TcpClient&& other)            = delete;
            TcpClient& operator=(TcpClient&& other) = delete;
            TcpClient(const TcpClient&)             = delete;
            TcpClient& operator=(const TcpClient&)  = delete;
            ~TcpClient()                            = default;
            void Connect(const std::string& host, int port) {
                this->host             = host;
                this->port             = port;
                sockets::SocketRet ret = m_client.connectTo(this->host.c_str(), this->port);
                this->connected.store(ret.m_success);
                if (ret.m_success) {
                    if (this->onConnectClb != nullptr) {
                        this->onConnectClb();
                    }
                    // this->server->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_CONNECTED, this->server);
                } else {
                    //            this->server->disconnect_reason = ret.m_msg;
                    //            this->server->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this->server);
                    //           this->m_client.finish();
                    this->disconnect_reason = ret.m_msg;
                    if (this->onDisconnectClb != nullptr) {
                        this->onDisconnectClb(this->disconnect_reason);
                    }
                    this->stop();
                }
            }
            void onReceiveData(const char* data, size_t size);
            void onDisconnect(const sockets::SocketRet& ret);
            void sendMsg(const char* data, size_t len);
            bool IsConnected() { return this->connected.load(); }
            std::string GetDisconnectReason() { return this->disconnect_reason; }
            inline sd_gui_utils::networks::Packet getPacket(size_t Id) {
                if (receivedPackets.find(Id) != receivedPackets.end()) {
                    return receivedPackets[Id];
                }
                return sd_gui_utils::networks::Packet();
            }

            inline void stop() {
                this->m_client.finish();
                this->connected.store(false);
            }
        };
    }
}
#endif  // _LIBS_TCPCLIENT_H_