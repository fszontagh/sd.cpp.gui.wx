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
    this->parent->sendLogEvent("Client " + std::to_string(idx) + " sent: " + std::to_string(len), wxLOG_Debug);
}
void SocketApp::sendMsg(int idx, const sd_gui_utils::networks::Packet& packet) {
    nlohmann::json j = packet;
    std::cout << "Sending: " << j.dump(2) << std::endl;
    std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(packet);
    const char* cbor_data            = reinterpret_cast<const char*>(v_cbor.data());
    size_t size                      = v_cbor.size();
    this->sendMsg(idx, cbor_data, size);
}

void SocketApp::onReceiveClientData(const sockets::ClientHandle& client, const char* data, size_t size) {
    std::string str(reinterpret_cast<const char*>(data), size);
    this->parent->sendLogEvent("Client " + std::to_string(client) + " Rcvd: " + str, wxLOG_Debug);
    this->parseMsg(client, data, size);
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

void SocketApp::parseMsg(int idx, const char* data, size_t size) {
    std::string str(data, size);
    // parse from cbor to struct Packet
    try {
        nlohmann::json json = nlohmann::json::from_cbor(str);
        auto packet         = json.get<sd_gui_utils::networks::Packet>();
        if (packet.version != SD_GUI_VERSION) {
            this->sendMsg(idx, sd_gui_utils::networks::Packet(sd_gui_utils::networks::PacketType::RESPONSE, sd_gui_utils::networks::PacketParam::ERROR, "Version Mismatch, server: " + std::string(SD_GUI_VERSION) + ", client: " + packet.version));
            this->parent->sendLogEvent(wxString::Format("Version Mismatch, server: %s, client: %s", SD_GUI_VERSION, packet.version), wxLOG_Error);
            this->DisconnectClient(idx);
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
                    if (this->parent->configData->authkey == packet.data) {
                        this->m_clientInfo[idx].apikey = packet.data;
                    }else{
                        this->sendMsg(idx, sd_gui_utils::networks::Packet(sd_gui_utils::networks::PacketType::RESPONSE, sd_gui_utils::networks::PacketParam::ERROR, "Authentication Failed"));
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