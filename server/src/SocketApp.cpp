#include "SocketApp.h"
#include "libs/SnowFlakeIdGenerarot.hpp"
SocketApp::SocketApp(const char* listenAddr, uint16_t port, TerminalApp* parent)
    : m_socketOpt({sd_gui_utils::TCP_TX_BUFFER_SIZE, sd_gui_utils::TCP_RX_BUFFER_SIZE, listenAddr}), m_server(*this, &m_socketOpt), parent(parent) {
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
        } else {
            this->m_clientInfo.at(idx).tx += len;
        }
    } else {
        this->parent->sendLogEvent("Client " + std::to_string(idx) + " doesn't exist", wxLOG_Warning);
    }
    auto fsize = sd_gui_utils::formatbytes(len);
    // wxString logmsg = wxString::Format("Sent to client id: %d size: %.1f %s", idx, fsize.first, fsize.second);
    // this->parent->sendLogEvent(logmsg, wxLOG_Debug);
}
size_t SocketApp::sendMsg(int idx, sd_gui_utils::networks::Packet& packet) {
    if (idx != 0 && this->m_clientInfo.contains(idx)) {
        packet.client_id = this->m_clientInfo[idx].client_id > 0 ? this->m_clientInfo[idx].client_id : 0;
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
                this->parent->sendLogEvent("Received packet expected size: " + std::to_string(this->expected_size), wxLOG_Debug);
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

    std::cout << "SocketApp::onReceiveClientData Expected size: " << this->expected_size
              << " Remaining size: " << size
              << " Buffer size: " << this->buffer.size() << std::endl;
}

void SocketApp::onClientConnect(const sockets::ClientHandle& client) {
    std::string ipAddr;
    uint16_t port;
    bool connected;
    if (m_server.getClientInfo(client, ipAddr, port, connected)) {
        this->parent->sendLogEvent("Client " + std::to_string(client) + " connected from " + ipAddr + ":" + std::to_string(port));
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            // generate an id
            // auto idGen                 = sd_gui_utils::SnowflakeIDGenerator(std::to_string(client), ipAddr, port);
            // idGen.generateID(this->m_clientInfo.size()
            this->m_clientInfo[client] = {ipAddr, port, client, wxGetLocalTime()};
        }
        sd_gui_utils::networks::Packet auth_required_packet;
        auth_required_packet.type  = sd_gui_utils::networks::Packet::Type::REQUEST_TYPE;
        auth_required_packet.param = sd_gui_utils::networks::Packet::Param::PARAM_AUTH;
        this->sendMsg(client, auth_required_packet);
    }
}

void SocketApp::onClientDisconnect(const sockets::ClientHandle& client, const sockets::SocketRet& ret) {
    this->parent->sendLogEvent("Client " + std::to_string(client) + " disconnected: " + ret.m_msg);
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (this->m_clientInfo.contains(client)) {
            this->m_clientInfo.at(client).disconnected_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->parent->sendDisconnectEvent(this->m_clientInfo.at(client));
            this->m_clientInfo.erase(client);
        }
    }
}

void SocketApp::OnTimer() {
    auto it = this->m_clientInfo.begin();
    while (it != this->m_clientInfo.end()) {
        if ((wxGetLocalTime() - it->second.connected_at) > this->parent->configData->unauthorized_timeout && it->second.apikey.empty()) {
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                this->parent->sendLogEvent(wxString::Format("Unauthorized client %d disconnected due to inactivity %s:%d", it->second.idx, it->second.host, it->second.port), wxLOG_Warning);
                this->m_server.deleteClient(it->second.idx);
                it = this->m_clientInfo.erase(it);
            }
        } else {
            if (it->second.last_keepalive == 0 || (wxGetLocalTime() - it->second.last_keepalive) > this->parent->configData->tcp_keepalive) {
                it->second.last_keepalive = wxGetLocalTime();
                auto keepAlivePacket      = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::REQUEST_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_KEEPALIVE);
                // keepAlivePacket.SetData(std::string(SD_GUI_VERSION));
                auto packet_size = this->sendMsg(it->second.idx, keepAlivePacket);
                this->parent->sendLogEvent("Keepalive sent to " + it->second.host + ":" + std::to_string(it->second.port) + " packet size: " + std::to_string(packet_size) + "", wxLOG_Debug);
                this->parent->sendOnTimerEvent(it->second);
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
            this->parent->sendLogEvent(wxString::Format("Version Mismatch, server: %s, client: %s", SD_GUI_VERSION, packet.version), wxLOG_Error);
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
                    this->parent->sendLogEvent("Got auth key: \n" + packetData + " our key: \n" + this->parent->configData->authkey, wxLOG_Debug);
                    if (packet.client_id > 0) {
                        this->m_clientInfo[packet.source_idx].client_id = packet.client_id;
                    }
                    if (this->parent->configData->authkey.compare(packetData) == 0) {
                        this->m_clientInfo[packet.source_idx].apikey = packetData;
                        if (packet.client_id > 0) {
                            this->m_clientInfo[packet.source_idx].client_id = packet.client_id;
                        }

                        auto responsePacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_AUTH);
                        responsePacket.SetData("Authentication done");
                        responsePacket.server_id   = this->parent->configData->server_id;
                        responsePacket.client_id   = this->m_clientInfo[packet.source_idx].client_id;
                        responsePacket.server_name = this->parent->configData->server_name;
                        this->sendMsg(packet.source_idx, responsePacket);
                        this->parent->sendLogEvent("Client authenticated: " + std::to_string(packet.source_idx), wxLOG_Info);
                    } else {
                        auto errorPacket = sd_gui_utils::networks::Packet(sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, sd_gui_utils::networks::Packet::Param::PARAM_ERROR);
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
        this->parent->sendDisconnectEvent(this->m_clientInfo[idx]);
        if (this->m_server.deleteClient(idx)) {
            this->m_clientInfo.erase(idx);
        }
    }
}
