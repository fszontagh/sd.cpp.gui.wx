#include "TcpClient.h"
namespace sd_gui_utils {
    inline namespace networks {
        void TcpClient::sendMsg(const char* data, size_t len) {
            auto ret = m_client.sendMsg(data, len);
            if (!ret.m_success) {
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
                    std::lock_guard<std::mutex> lock(callbackMutex_);
                    connectClb = callbacks_.onConnectClb;
                    if (connectClb) {
                        connectClb();
                    }
                }
            }
            return ret.m_success;
        }

        void TcpClient::onReceiveData(const char* data, size_t size) {
            while (size > 0) {
                if (this->expected_size == 0) {
                    // Ellenőrizzük, hogy elegendő adat érkezett-e a csomag méretének kiolvasásához
                    if (size >= sd_gui_utils::networks::PACKET_SIZE_LENGTH) {
                        memcpy(&this->expected_size, data, sd_gui_utils::networks::PACKET_SIZE_LENGTH);

                        size -= sd_gui_utils::networks::PACKET_SIZE_LENGTH;
                        data += sd_gui_utils::networks::PACKET_SIZE_LENGTH;
                    } else {
                        this->buffer.insert(this->buffer.end(), data, data + size);
                        break;
                    }
                }

                size_t remaining_size = this->expected_size - this->buffer.size();

                if (size >= remaining_size) {
                    // Teljes csomag érkezett meg vagy több adat van a szükségesnél
                    this->buffer.insert(this->buffer.end(), data, data + remaining_size);

                    // Csomag feldolgozása
                    auto packet = sd_gui_utils::networks::Packet::DeSerialize(this->buffer.data(), this->buffer.size());
                    this->HandlePackets(packet);

                    this->buffer.clear();
                    this->expected_size = 0;

                    size -= remaining_size;
                    data += remaining_size;
                } else {
                    this->buffer.insert(this->buffer.end(), data, data + size);
                    break;
                }
            }
        }

        void TcpClient::HandlePackets(Packet& packet) {
            if (packet.type == sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE) {
                if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_ERROR) {
                    this->disconnect_reason = packet.GetData<std::string>();
                    TcpClientOnError onErrorClb;
                    {
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
                size_t packet_id = this->receivedPackets.size();
                {
                    std::lock_guard<std::mutex> lock(packetMutex_);
                    packet.packet_added_time         = std::chrono::system_clock::now().time_since_epoch().count();
                    this->receivedPackets[packet_id] = packet;
                }
                TcpClientOnMessage onMessageClb;
                {
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
                        std::lock_guard<std::mutex> lock(callbackMutex_);
                        onAuthRequestClb = callbacks_.onAuthRequestClb;
                        if (onAuthRequestClb) {
                            onAuthRequestClb();
                        }
                    }
                    return;
                }
                if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_KEEPALIVE) {
                    this->cleanUpPackets();
                }
            }
        }

        void TcpClient::onDisconnect(const sockets::SocketRet& ret) {
            this->connected.store(false);
            this->disconnect_reason = ret.m_msg;

            TcpClientOntDisconnect disconnectClb;
            {
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
            auto raw_packet                                              = sd_gui_utils::networks::Packet::Serialize(packet);
            size_t packet_size                                           = raw_packet.second;
            char size_header[sd_gui_utils::networks::PACKET_SIZE_LENGTH] = {0};
            memcpy(size_header, &packet_size, sizeof(packet_size));
            std::vector<char> full_packet(sd_gui_utils::networks::PACKET_SIZE_LENGTH + raw_packet.second);
            memcpy(full_packet.data(), size_header, sd_gui_utils::networks::PACKET_SIZE_LENGTH);                           // Header másolása
            memcpy(full_packet.data() + sd_gui_utils::networks::PACKET_SIZE_LENGTH, raw_packet.first, raw_packet.second);  // Adat másolása

            this->sendMsg(full_packet.data(), full_packet.size());

            delete[] raw_packet.first;
        }
    }
}