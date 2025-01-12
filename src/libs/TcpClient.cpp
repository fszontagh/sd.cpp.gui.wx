#include "TcpClient.h"
namespace sd_gui_utils {
    inline namespace networks {
        void TcpClient::sendMsg(const char* data, size_t len) {
            auto ret = m_client.sendMsg(data, len);
            if (!ret.m_success) {
                std::cout << "Send Error: " << ret.m_msg << "\n";

                TcpClientOnError onErrorClb;
                {
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    onErrorClb = callbacks_.onErrorClb;
                    if (onErrorClb) {
                        onErrorClb(ret.m_msg);
                    }
                }
            }
        }

        bool TcpClient::Connect(const std::string& host, int port) {
            this->host             = host;
            this->port             = port;
            sockets::SocketRet ret = m_client.connectTo(this->host.c_str(), this->port);
            this->connected.store(ret.m_success);

            if (ret.m_success) {
                TcpClientOnConnect connectClb;
                {
                    std::cout << "Lock on connectClb" << std::endl;
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    connectClb = callbacks_.onConnectClb;
                    if (connectClb) {
                        connectClb();
                    }
                }

            } /* else {
                 this->disconnect_reason = ret.m_msg;
                 TcpClientOntDisconnect disconnectClb;
                 {
                     std::cout << "Lock on disconnectClb" << std::endl;
                     std::lock_guard<std::mutex> lock(callbackMutex_);
                     disconnectClb = callbacks_.onDisconnectClb;
                     if (disconnectClb) {
                         disconnectClb(this->disconnect_reason);
                     }
                 }

                 this->stop();
             }*/
            return ret.m_success;
        }

        void TcpClient::onReceiveData(const char* data, size_t size) {
            if (this->expected_size == 0 && size >= sizeof(size_t)) {
                memcpy(&this->expected_size, data, sizeof(this->expected_size));

                size -= sizeof(this->expected_size);
                data += sizeof(this->expected_size);
                if (size > 0) {
                    this->buffer.insert(this->buffer.end(), data, data + size);
                }
                std::cout << "Expected size: " << this->expected_size << "\n";
            } else if (this->expected_size > 0) {
                this->buffer.insert(this->buffer.end(), data, data + size);
            }

            if (this->buffer.size() == this->expected_size && this->expected_size > 0) {
                auto packet = sd_gui_utils::networks::Packet::DeSerialize(this->buffer.data(), this->buffer.size());

                this->HandlePackets(packet);
                this->buffer.clear();
                this->expected_size = 0;
            }
        }
        void TcpClient::HandlePackets(Packet& packet) {
            std::cout << "Handling packet: " << (int)packet.type << " Param:" << (int)packet.param << "\n";
            if (packet.type == sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE) {
                if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_ERROR) {
                    this->disconnect_reason = packet.GetData<std::string>();
                    TcpClientOnError onErrorClb;
                    {
                        std::cout << "Lock on onErrorClb" << std::endl;
                        std::lock_guard<std::mutex> lock(callbackMutex_);
                        onErrorClb = callbacks_.onErrorClb;
                        if (onErrorClb) {
                            onErrorClb(this->disconnect_reason);
                        }
                    }

                    return;
                }
                // if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST) {

                //}
                size_t packet_id                 = this->receivedPackets.size();
                this->receivedPackets[packet_id] = packet;
                TcpClientOnMessage onMessageClb;
                {
                    std::cout << "Lock on onMessageClb" << std::endl;
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    onMessageClb = callbacks_.onMessageClb;
                    if (onMessageClb) {
                        onMessageClb(packet_id);
                    }
                }

                return;
            }
            if (packet.type == sd_gui_utils::networks::Packet::Type::REQUEST_TYPE) {
                if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_AUTH) {
                    TcpClientOnAuthRequest onAuthRequestClb;
                    {
                        std::cout << "Lock on onAuthRequestClb" << std::endl;
                        std::lock_guard<std::mutex> lock(callbackMutex_);
                        onAuthRequestClb = callbacks_.onAuthRequestClb;
                        if (onAuthRequestClb) {
                            onAuthRequestClb();
                        }
                    }
                    return;
                }
            }
        }

        void TcpClient::onDisconnect(const sockets::SocketRet& ret) {
            this->connected.store(false);
            this->disconnect_reason = ret.m_msg;

            TcpClientOntDisconnect disconnectClb;
            {
                std::cout << "Lock on disconnectClb" << std::endl;
                std::lock_guard<std::mutex> lock(callbackMutex_);
                disconnectClb = callbacks_.onDisconnectClb;
            }
            this->disconnect_reason = ret.m_msg;
            if (disconnectClb) {
                disconnectClb(this->disconnect_reason);
            }
            this->stop();
        }

        void TcpClient::sendMsg(const sd_gui_utils::networks::Packet& packet) {
            auto raw_packet    = sd_gui_utils::networks::Packet::Serialize(packet);
            size_t packet_size = raw_packet.second;
            this->sendMsg(reinterpret_cast<const char*>(&packet_size), sizeof(packet_size));
            this->sendMsg(raw_packet.first, raw_packet.second);
            delete[] raw_packet.first;
        }
    }
}