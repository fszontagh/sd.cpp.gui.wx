#ifndef _LIBS_TCPCLIENT_H_
#define _LIBS_TCPCLIENT_H_

#include "../network/packets.h"
// #include "../helpers/EventQueue.hpp"

namespace sd_gui_utils {
    inline namespace networks {
        using TcpClientOnMessage     = std::function<void(int)>;
        using TcpClientOnConnect     = std::function<void()>;
        using TcpClientOntDisconnect = std::function<void(std::string)>;
        using TcpClientOnError       = std::function<void(const std::string&)>;
        using TcpClientOnStop        = std::function<void()>;
        using TcpClientOnAuthRequest = std::function<void()>;

        class TcpClient {
            sockets::SocketOpt m_socketOpt;
            sockets::TcpClient<TcpClient> m_client;
            std::unordered_map<size_t, sd_gui_utils::networks::Packet> receivedPackets;
            std::vector<char> buffer;
            size_t expected_size = 0;
            void HandlePackets(sd_gui_utils::networks::Packet& packet);
            void sendMsg(const char* data, size_t len);
            std::atomic<bool> connected{false};
            std::string host, disconnect_reason;
            int port;
            mutable std::mutex callbackMutex_;
            struct Callbacks {
                TcpClientOnMessage onMessageClb         = nullptr;
                TcpClientOnConnect onConnectClb         = nullptr;
                TcpClientOntDisconnect onDisconnectClb  = nullptr;
                TcpClientOnError onErrorClb             = nullptr;
                TcpClientOnStop onStopClb               = nullptr;
                TcpClientOnAuthRequest onAuthRequestClb = nullptr;
            };
            Callbacks callbacks_;

        public:
            TcpClient()
                : m_socketOpt({sd_gui_utils::TCP_TX_BUFFER_SIZE, sd_gui_utils::TCP_RX_BUFFER_SIZE}), m_client(*this, &m_socketOpt) {};

            TcpClient(TcpClient&& other)            = delete;
            TcpClient& operator=(TcpClient&& other) = delete;
            TcpClient(const TcpClient&)             = delete;
            TcpClient& operator=(const TcpClient&)  = delete;
            ~TcpClient() {
                this->stop();
            }
            bool Connect(const std::string& host, int port);
            void onReceiveData(const char* data, size_t size);
            void onDisconnect(const sockets::SocketRet& ret);
            void sendMsg(const sd_gui_utils::networks::Packet& packet);
            bool IsConnected() { return this->connected.load(); }
            inline std::string GetDisconnectReason() { return this->disconnect_reason; }
            inline sd_gui_utils::networks::Packet getPacket(size_t Id) {
                if (receivedPackets.find(Id) != receivedPackets.end()) {
                    return receivedPackets[Id];
                }
                return sd_gui_utils::networks::Packet();
            }

            inline void stop() {
                std::cout << "TcpClient stop" << std::endl;
                this->m_client.finish();
                this->connected.store(false);

                TcpClientOnStop onStopClb;
                {
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    onStopClb = callbacks_.onStopClb;
                }
                if (onStopClb) {
                    onStopClb();
                }
            }
            void SetOnConnectCallback(TcpClientOnConnect callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnConnectCallback" << std::endl;
                callbacks_.onConnectClb = std::move(callback);
            }

            void SetOnMessageCallback(TcpClientOnMessage callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnMessageCallback" << std::endl;
                callbacks_.onMessageClb = std::move(callback);
            }

            void SetOnDisconnectCallback(TcpClientOntDisconnect callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnDisconnectCallback" << std::endl;
                callbacks_.onDisconnectClb = std::move(callback);
            }

            void SetOnErrorCallback(TcpClientOnError callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnErrorCallback" << std::endl;
                callbacks_.onErrorClb = std::move(callback);
            }

            void SetOnStopCallback(TcpClientOnStop callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnStopCallback" << std::endl;
                callbacks_.onStopClb = std::move(callback);
            }
            void SetOnAuthRequestCallback(TcpClientOnAuthRequest callback) {
                std::lock_guard<std::mutex> lock(callbackMutex_);
                std::cout << "SetOnAuthRequestCallback" << std::endl;
                callbacks_.onAuthRequestClb = std::move(callback);
            }
        };
    }
}
#endif  // _LIBS_TCPCLIENT_H_