#include "SocketApp.h"
#include "libs/SnowFlakeIdGenerarot.hpp"
SocketApp::SocketApp(const char* listenAddr, uint16_t port, TerminalApp* parent)
    : m_socketOpt({sd_gui_utils::TCP_TX_BUFFER_SIZE, sd_gui_utils::TCP_RX_BUFFER_SIZE, listenAddr}), m_server(*this, &m_socketOpt), parent(parent) {
    sockets::SocketRet ret = m_server.start(port);
    this->needToRun.store(ret.m_success);
    if (ret.m_success) {
        this->parent->SendLogEvent("Server listening on " + std::string(listenAddr) + ":" + std::to_string(port));
    } else {
        this->parent->SendLogEvent("Server error: " + ret.m_msg, wxLOG_Error);
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
            this->parent->SendLogEvent("Broadcast send Error: " + ret.m_msg, wxLOG_Error);
        }
    } else if (this->m_clientInfo.contains(idx)) {
        auto ret = m_server.sendClientMessage(idx, data, len);
        if (!ret.m_success) {
            this->parent->SendLogEvent("Send Error: " + ret.m_msg, wxLOG_Error);
        } else {
            this->m_clientInfo.at(idx).tx += len;
        }
    } else {
        this->parent->SendLogEvent("Client " + std::to_string(idx) + " doesn't exist", wxLOG_Warning);
    }
    auto fsize = sd_gui_utils::formatbytes(len);
    // wxString logmsg = wxString::Format("Sent to client id: %d size: %.1f %s", idx, fsize.first, fsize.second);
    // this->parent->sendLogEvent(logmsg, wxLOG_Debug);
}
size_t SocketApp::sendMsg(int idx, sd_gui_utils::networks::Packet& packet) {
    if (idx != 0 && this->m_clientInfo.contains(idx)) {
        packet.client_session_id = this->m_clientInfo[idx].client_id > 0 ? this->m_clientInfo[idx].client_id : 0;
    }

    auto raw_packet    = sd_gui_utils::networks::Packet::Serialize(packet);
    size_t packet_size = raw_packet.second;

    char size_header[sd_gui_utils::networks::PACKET_SIZE_LENGTH] = {0};
    memcpy(size_header, &packet_size, sizeof(packet_size));

    std::vector<char> full_packet(sd_gui_utils::networks::PACKET_SIZE_LENGTH + raw_packet.second);
    memcpy(full_packet.data(), size_header, sd_gui_utils::networks::PACKET_SIZE_LENGTH);                           // Header másolása
    memcpy(full_packet.data() + sd_gui_utils::networks::PACKET_SIZE_LENGTH, raw_packet.first, raw_packet.second);  // Adat másolása

    this->sendMsg(idx, full_packet.data(), full_packet.size());

    delete[] raw_packet.first;
    return packet_size;
}

void SocketApp::onReceiveClientData(const sockets::ClientHandle& client, const char* data, size_t size) {
    while (size > 0) {
        if (this->expected_size == 0) {
            if (size >= sd_gui_utils::networks::PACKET_SIZE_LENGTH) {
                memcpy(&this->expected_size, data, sd_gui_utils::networks::PACKET_SIZE_LENGTH);

                size -= sd_gui_utils::networks::PACKET_SIZE_LENGTH;
                data += sd_gui_utils::networks::PACKET_SIZE_LENGTH;
                this->parent->SendLogEvent("Received packet expected size: " + std::to_string(this->expected_size), wxLOG_Debug);
            } else {
                this->buffer.insert(this->buffer.end(), data, data + size);
                break;
            }
        }

        size_t remaining_size = this->expected_size - this->buffer.size();

        if (size >= remaining_size) {
            this->buffer.insert(this->buffer.end(), data, data + remaining_size);

            auto packet       = sd_gui_utils::networks::Packet::DeSerialize(this->buffer.data(), this->buffer.size());
            packet.source_idx = client;
            this->parseMsg(packet);
            this->m_clientInfo.at(client).rx += this->expected_size;

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

void SocketApp::onClientConnect(const sockets::ClientHandle& client) {
    std::string ipAddr;
    uint16_t port;
    bool connected;
    if (m_server.getClientInfo(client, ipAddr, port, connected)) {
        this->parent->SendLogEvent("Client " + std::to_string(client) + " connected from " + ipAddr + ":" + std::to_string(port));
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            // generate an id
            // auto idGen                 = sd_gui_utils::SnowflakeIDGenerator(std::to_string(client), ipAddr, port);
            // idGen.generateID(this->m_clientInfo.size()
            clientInfo info            = {ipAddr, port, client, wxGetLocalTime()};
            info.client_id             = this->m_clientInfo.size() + 1;
            this->m_clientInfo[client] = info;
        }
        sd_gui_utils::networks::Packet auth_required_packet;
        auth_required_packet.type  = sd_gui_utils::networks::Packet::Type::REQUEST_TYPE;
        auth_required_packet.param = sd_gui_utils::networks::Packet::Param::PARAM_AUTH;
        this->sendMsg(client, auth_required_packet);
    }
}

void SocketApp::onClientDisconnect(const sockets::ClientHandle& client, const sockets::SocketRet& ret) {
    this->parent->SendLogEvent("Client " + std::to_string(client) + " disconnected: " + ret.m_msg);
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (this->m_clientInfo.contains(client)) {
            this->m_clientInfo.at(client).disconnected_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            // this->parent->SendDisconnectEvent(this->m_clientInfo.at(client));
            this->parent->StoreClientStats(this->m_clientInfo.at(client));
            this->m_clientInfo.erase(client);
        }
    }
}

void SocketApp::OnTimer() {
    if (this->m_clientInfo.empty()) {
        return;
    }

    const time_t currentTime       = wxGetLocalTime();
    const auto unauthorizedTimeout = this->parent->configData->unauthorized_timeout;
    const auto tcpKeepalive        = this->parent->configData->tcp_keepalive;

    auto it = this->m_clientInfo.begin();
    while (it != this->m_clientInfo.end()) {
        auto& client = it->second;

        if ((currentTime - client.connected_at) > unauthorizedTimeout && client.apikey.empty()) {
            this->parent->SendLogEvent(wxString::Format(
                                           "Unauthorized client %d disconnected due to inactivity %s:%d",
                                           client.idx, client.host, client.port),
                                       wxLOG_Warning);

            {
                std::lock_guard<std::mutex> guard(m_mutex);
                this->m_server.deleteClient(client.idx);
                it = this->m_clientInfo.erase(it);
            }
        } else {
            if (client.last_keepalive == 0 || (currentTime - client.last_keepalive) > tcpKeepalive) {
                client.last_keepalive = currentTime;
                auto keepAlivePacket  = sd_gui_utils::networks::Packet(
                    sd_gui_utils::networks::Packet::Type::REQUEST_TYPE,
                    sd_gui_utils::networks::Packet::Param::PARAM_KEEPALIVE);

                auto packet_size = this->sendMsg(client.idx, keepAlivePacket);
                this->parent->SendLogEvent(
                    "Keepalive sent to " + client.host + ":" + std::to_string(client.port) +
                        " packet size: " + std::to_string(packet_size),
                    wxLOG_Debug);

                this->parent->StoreClientStats(client);  // Store client info into JSON
            }
            ++it;
        }
    }
}

void SocketApp::parseMsg(sd_gui_utils::networks::Packet& packet) {
    // parse from cbor to struct Packet
    try {
        // store the client id which is saved at the client if already connected once
        //  if (packet.client_id > 0 && this->m_clientInfo.contains(packet.source_idx)) {
        //      auto oldInfo = this->parent->ReReadClientInfo(packet.client_id, this->m_clientInfo[packet.source_idx].client_id != packet.client_id);
        //      this->m_clientInfo[packet.source_idx].copyFrom(oldInfo);
        //  }
        if (packet.version != SD_GUI_VERSION) {
            auto errorPacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_ERROR);
            errorPacket.SetData("Version Mismatch, server: " + std::string(SD_GUI_VERSION) + ", client: " + packet.version);
            this->sendMsg(packet.source_idx, errorPacket);
            this->parent->SendLogEvent(wxString::Format("Version Mismatch, server: %s, client: %s", SD_GUI_VERSION, packet.version), wxLOG_Error);
            this->DisconnectClient(packet.source_idx);
            return;
        }

        if (packet.type == sd_gui_utils::networks::Packet::Type::REQUEST_TYPE || packet.type == sd_gui_utils::networks::Packet::Type::DELETE_TYPE) {
            this->parent->ProcessReceivedSocketPackages(packet);
        }
        // handle the response to the auth request
        if (packet.type == sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE) {
            if (packet.param == sd_gui_utils::networks::Packet::Param::PARAM_AUTH) {
                {
                    // std::lock_guard<std::mutex> guard(m_mutex);
                    std::string packetData = packet.GetData<std::string>();
                    this->parent->SendLogEvent("Got auth key: \n" + packetData + " our key: \n" + this->parent->configData->authkey, wxLOG_Debug);
                    if (packet.client_session_id > 0) {
                        this->m_clientInfo[packet.source_idx].client_id = packet.client_session_id;
                    } else {
                        this->m_clientInfo[packet.source_idx].client_id = this->GetNextClientId();
                    }
                    if (this->parent->configData->authkey.compare(packetData) == 0) {
                        this->m_clientInfo[packet.source_idx].apikey = packetData;

                        auto responsePacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_AUTH);
                        responsePacket.SetData("Authentication done");
                        responsePacket.server_id         = this->parent->configData->server_id;
                        responsePacket.client_session_id = this->m_clientInfo[packet.source_idx].client_id;
                        responsePacket.server_name       = this->parent->configData->server_name;
                        this->sendMsg(packet.source_idx, responsePacket);
                        this->parent->SendLogEvent("Client authenticated: " + std::to_string(packet.source_idx), wxLOG_Info);
                        this->parent->StoreClientStats(this->m_clientInfo[packet.source_idx]);
                    } else {
                        auto errorPacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_ERROR);
                        errorPacket.SetData("Authentication Failed");
                        this->sendMsg(packet.source_idx, errorPacket);
                        this->parent->SendLogEvent("Authentication Failed, ip: " + this->m_clientInfo[packet.source_idx].host + " port: " + std::to_string(this->m_clientInfo[packet.source_idx].port) + " key: " + this->m_clientInfo[packet.source_idx].apikey + "", wxLOG_Error);
                    }
                }
            }
        }
    } catch (nlohmann::json::exception& e) {
        this->parent->SendLogEvent(wxString::Format("Json Parse Error: %s", e.what()), wxLOG_Error);
    }
}

void SocketApp::DisconnectClient(int idx) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (this->m_clientInfo.contains(idx)) {
        this->parent->StoreClientStats(this->m_clientInfo[idx]);
        // this->parent->SendDisconnectEvent(this->m_clientInfo[idx]);
        if (this->m_server.deleteClient(idx)) {
            this->m_clientInfo.erase(idx);
        }
    }
}
void SocketApp::stop() {
    this->needToRun.store(false);
}
int SocketApp::GetNextClientId() {
    int max = 0;
    // read all client info from files
    const auto dir = this->parent->configData->GetClientDataPath();
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".json") {
            std::string fileName = entry.path().filename().string();
            std::string clientId = fileName.substr(0, fileName.length() - 5);
            int id               = std::stoi(clientId);
            if (id > max) {
                max = id;
            }
        }
    }
    return max + 1;
}
