#include "TcpClient.h"
namespace sd_gui_utils::networks {
    TcpClient::TcpClient(sd_gui_utils::sdServer* server, wxEvtHandler* evt)
        : m_client(*this), server(server), evt(evt) {
        sockets::SocketRet ret  = m_client.connectTo(this->server->host.c_str(), this->server->port);
        this->server->connected = ret.m_success;
        if (ret.m_success) {
            this->server->disconnect_reason = "";
            std::cout << "Connected to " << this->server->host << ":" << this->server->port << "\n";
            this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_CONNECTED, this->server);
        } else {
            this->server->disconnect_reason = ret.m_msg;
            this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this->server);
            this->m_client.finish();
        }
    }

    void TcpClient::sendMsg(const char* data, size_t len) {
        auto ret = m_client.sendMsg(data, len);
        if (!ret.m_success) {
            std::cout << "Send Error: " << ret.m_msg << "\n";
        }
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
    void TcpClient::HandlePackets(sd_gui_utils::networks::Packet& packet) {
        if (packet.type == sd_gui_utils::networks::PacketType::RESPONSE) {
            if (packet.param == sd_gui_utils::networks::PacketParam::ERROR) {
                this->server->disconnect_reason = packet.GetData<std::string>();
                this->server->needToRun         = false;
                this->server->enabled           = false;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_ERROR, this->server);
            }
            if (packet.param == sd_gui_utils::networks::PacketParam::MODEL_LIST) {
                size_t packet_id                 = this->receivedPackets.size();
                this->receivedPackets[packet_id] = packet;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_MODEL_LIST_UPDATE, server, packet_id);
            }
        }
        if (packet.type == sd_gui_utils::networks::PacketType::REQUEST) {
            if (packet.param == sd_gui_utils::networks::PacketParam::AUTH) {
                // get the auth token from secret store
                wxSecretStore store  = wxSecretStore::GetDefault();
                wxString serviceName = wxString::Format(wxT("%s/%s_%d"), PROJECT_NAME, wxString::FromUTF8Unchecked(server->host), server->port);
                wxString username;
                wxSecretValue authkey;

                if (store.IsOk() && store.Load(serviceName, username, authkey)) {
                    auto responsePacket  = sd_gui_utils::networks::Packet();
                    responsePacket.type  = sd_gui_utils::networks::PacketType::RESPONSE;
                    responsePacket.param = sd_gui_utils::networks::PacketParam::AUTH;
                    responsePacket.SetData(authkey.GetAsString().utf8_string());

                    this->sendMsg(responsePacket);
                    // send model list request
                    auto modelListPacket  = sd_gui_utils::networks::Packet();
                    modelListPacket.type  = sd_gui_utils::networks::PacketType::REQUEST;
                    modelListPacket.param = sd_gui_utils::networks::PacketParam::MODEL_LIST;
                    this->sendMsg(modelListPacket);
                } else {
                    this->server->disconnect_reason = "Authentication Failed";
                    this->server->needToRun         = false;
                    this->server->enabled           = false;
                    this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this->server);
                }
            }
        }
    }

    void TcpClient::onDisconnect(const sockets::SocketRet& ret) {
        this->server->connected = false;
        if (ret.m_msg.length() > 0) {
            this->server->disconnect_reason = ret.m_msg;
        }
        this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this->server);
    }

    void TcpClient::sendMsg(const sd_gui_utils::networks::Packet& packet) {
        auto raw_packet    = sd_gui_utils::networks::Packet::Serialize(packet);
        size_t packet_size = raw_packet.second;
        this->sendMsg(reinterpret_cast<const char*>(&packet_size), sizeof(packet_size));
        this->sendMsg(raw_packet.first, raw_packet.second);
        delete[] raw_packet.first;
    }
}  // namespace sd_gui_utils::networks