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
        std::string str(data, size);

        try {
            nlohmann::json json = nlohmann::json::from_cbor(str);
            auto packet         = json.get<sd_gui_utils::networks::Packet>();
            std::cout << "Received: " << json.dump(2) << std::endl;
            if (packet.type == sd_gui_utils::networks::PacketType::RESPONSE) {
                if (packet.param == sd_gui_utils::networks::PacketParam::ERROR) {
                    this->server->disconnect_reason = packet.data;
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
                        responsePacket.data  = authkey.GetAsString().utf8_string().c_str();
                        this->sendMsg(responsePacket);
                    } else {
                        this->server->disconnect_reason = "Authentication Failed";
                        this->server->needToRun         = false;
                        this->server->enabled           = false;
                        this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this->server);
                    }
                }
            }
        } catch (nlohmann::json::exception& e) {
            std::cout << "Json Parse Error: " << e.what() << "\n";
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
        std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(packet);
        const char* cbor_data            = reinterpret_cast<const char*>(v_cbor.data());
        size_t size                      = v_cbor.size();
        this->sendMsg(cbor_data, size);
    }
}  // namespace sd_gui_utils::networks