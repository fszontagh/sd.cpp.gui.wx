#include "SocketApp.h"
#include "TerminalApp.h"
SocketApp::SocketApp(const char* listenAddr, uint16_t port, TerminalApp* parent)
    : m_socketOpt({sockets::TX_BUFFER_SIZE, sockets::RX_BUFFER_SIZE, listenAddr}), m_server(*this, &m_socketOpt), parent(parent) {
    sockets::SocketRet ret = m_server.start(port);
    if (ret.m_success) {
        this->parent->sendLogEvent("Server started on  " + std::string(listenAddr) + ":" + std::to_string(port));
    } else {
        this->parent->sendLogEvent("Error: " + ret.m_msg, wxLOG_Error);
        this->needToRun = false;
    }
}

SocketApp::~SocketApp() {
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        this->m_clientInfo.clear();
    }
}

void SocketApp::sendMsg(int idx, const char* data, size_t len) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (idx == 0) {
        auto ret = m_server.sendBcast(data, len);
        if (!ret.m_success) {
            this->parent->sendLogEvent("Broadcast send Error: " + ret.m_msg, wxLOG_Error);
        }
    } else if (this->m_clientInfo.contains(idx)) {
        auto ret = m_server.sendClientMessage(idx, data, len);
        if (!ret.m_success) {
            this->parent->sendLogEvent("Send Error: " + ret.m_msg, wxLOG_Error);
        }
    } else {
        this->parent->sendLogEvent("Client " + std::to_string(idx) + " doesn't exist", wxLOG_Warning);
    }
    this->parent->sendLogEvent("Sent to client id: " + std::to_string(idx) + " sent size: " + std::to_string(len), wxLOG_Debug);
}
void SocketApp::sendMsg(int idx, const sd_gui_utils::networks::Packet& packet) {
    auto raw_packet    = sd_gui_utils::networks::Packet::Serialize(packet);
    size_t packet_size = raw_packet.second;
    this->sendMsg(idx, reinterpret_cast<const char*>(&packet_size), sizeof(packet_size));
    this->sendMsg(idx, raw_packet.first, raw_packet.second);
    delete[] raw_packet.first;
}

void SocketApp::onReceiveClientData(const sockets::ClientHandle& client, const char* data, size_t size) {
    if (this->expected_size == 0 && size >= sizeof(size_t)) {
        memcpy(&this->expected_size, data, sizeof(this->expected_size));

        size -= sizeof(this->expected_size);
        data += sizeof(this->expected_size);

        if (size > 0) {
            this->buffer.insert(this->buffer.end(), data, data + size);
        }
        this->parent->sendLogEvent("Received packet expected size: " + std::to_string(this->expected_size), wxLOG_Debug);
    } else if (this->expected_size > 0) {
        this->buffer.insert(this->buffer.end(), data, data + size);
    }

    if (this->buffer.size() == this->expected_size && this->expected_size > 0) {
        auto packet = sd_gui_utils::networks::Packet::DeSerialize(this->buffer.data(), this->buffer.size());
        packet.source_idx = client;

        this->parseMsg(packet);

        this->buffer.clear();
        this->expected_size = 0;
    }
}

void SocketApp::onClientConnect(const sockets::ClientHandle& client) {
    std::string ipAddr;
    uint16_t port;
    bool connected;
    if (m_server.getClientInfo(client, ipAddr, port, connected)) {
        this->parent->sendLogEvent("Client " + std::to_string(client) + " connected from " + ipAddr + ":" + std::to_string(port));
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            this->m_clientInfo[client] = {ipAddr, port, client, wxGetLocalTime()};
        }
        sd_gui_utils::networks::Packet auth_required_packet;
        auth_required_packet.type  = sd_gui_utils::networks::PacketType::REQUEST;
        auth_required_packet.param = sd_gui_utils::networks::PacketParam::AUTH;
        this->sendMsg(client, auth_required_packet);
    }
}

void SocketApp::onClientDisconnect(const sockets::ClientHandle& client, const sockets::SocketRet& ret) {
    this->parent->sendLogEvent("Client " + std::to_string(client) + " disconnected: " + ret.m_msg);
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_clientInfo.erase(client);
    }
}

void SocketApp::OnTimer() {
    std::lock_guard<std::mutex> guard(m_mutex);

    auto it = this->m_clientInfo.begin();
    while (it != this->m_clientInfo.end()) {
        if ((wxGetLocalTime() - it->second.connected_at) > this->parent->configData->unauthorized_timeout && it->second.apikey.empty()) {
            this->parent->sendLogEvent(wxString::Format("Unauthorized client %d disconnected due to inactivity %s:%d", it->second.idx, it->second.host, it->second.port), wxLOG_Warning);
            this->m_server.deleteClient(it->second.idx);
            it = this->m_clientInfo.erase(it);
        } else {
            ++it;
        }
    }
}

void SocketApp::parseMsg(sd_gui_utils::networks::Packet& packet) {
    // parse from cbor to struct Packet
    try {
        if (packet.version != SD_GUI_VERSION) {
            auto errorPacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::PacketType::RESPONSE, sd_gui_utils::networks::PacketParam::ERROR);
            errorPacket.SetData("Version Mismatch, server: " + std::string(SD_GUI_VERSION) + ", client: " + packet.version);
            this->sendMsg(packet.source_idx, errorPacket);
            this->parent->sendLogEvent(wxString::Format("Version Mismatch, server: %s, client: %s", SD_GUI_VERSION, packet.version), wxLOG_Error);
            this->DisconnectClient(packet.source_idx);
            return;
        }
        if (packet.type == sd_gui_utils::networks::PacketType::REQUEST) {
            this->parent->ProcessReceivedSocketPackages(packet);
        }
        // handle the response to the auth request
        if (packet.type == sd_gui_utils::networks::PacketType::RESPONSE) {
            if (packet.param == sd_gui_utils::networks::PacketParam::AUTH) {
                {
                    std::lock_guard<std::mutex> guard(m_mutex);
                    std::string packetData = packet.GetData<std::string>();
                    this->parent->sendLogEvent("Got auth key: " + packetData + " our key: " + this->parent->configData->authkey, wxLOG_Debug);

                    if (this->parent->configData->authkey.compare(packetData) == 0) {
                        this->m_clientInfo[packet.source_idx].apikey = packetData;
                        this->parent->sendLogEvent("Client authenticated: " + std::to_string(packet.source_idx), wxLOG_Info);
                    } else {
                        auto errorPacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::PacketType::RESPONSE, sd_gui_utils::networks::PacketParam::ERROR);
                        errorPacket.SetData("Authentication Failed");
                        this->sendMsg(packet.source_idx, errorPacket);
                        this->parent->sendLogEvent("Authentication Failed, ip: " + this->m_clientInfo[packet.source_idx].host + " port: " + std::to_string(this->m_clientInfo[packet.source_idx].port) + " key: " + this->m_clientInfo[packet.source_idx].apikey + "", wxLOG_Error);
                    }
                }
            }
        }
    } catch (nlohmann::json::exception& e) {
        this->parent->sendLogEvent(wxString::Format("Json Parse Error: %s", e.what()), wxLOG_Error);
    }
}

void SocketApp::DisconnectClient(int idx) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (this->m_clientInfo.contains(idx)) {
        if (this->m_server.deleteClient(idx)) {
            this->m_clientInfo.erase(idx);
        }
    }
}