#ifndef _NETWORK_PACKETS_H_
#define _NETWORK_PACKETS_H_

namespace sd_gui_utils {

    struct sdServer : public wxClientData {
        std::string name;  // name is get from the over tcp
        std::string host;
        bool authkey   = false;  // auth key is set in the secret
        int port       = 0;
        bool connected = false;  // we check if the server is connected and not needed to store it into config
        int row;
        bool enabled = false;
        bool IsOk() const { return !host.empty() && port > 0; }
        wxString GetName() const {
            if (this->name.empty()) {
                return wxString::Format("%s:%d", host.c_str(), port);
            }
            return wxString::FromUTF8Unchecked(this->name);
        }
        sdServer& operator=(const sdServer&) = default;
        sdServer(const std::string& host, int port)
            : host(host), port(port) {}
        // copy constructor
        sdServer(const sdServer& other) = default;
        sdServer() {}
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(sdServer, name, host, authkey, port, connected, row, enabled)

    namespace networks {

        enum class PacketType {
            REQUEST,
            RESPONSE
        };
        enum class PackaetParam {
            ERROR,
            INFO,
            TEXT,
            IMAGE,
            FILE,
            AUTH
        };

        struct Packet {
            Packet(sd_gui_utils::networks::PacketType type, sd_gui_utils::networks::PackaetParam param, const std::string& data = "")
                : type(type), param(param), data(data), size(data.size()) {}
            Packet() = default;
            sd_gui_utils::networks::PacketType type;
            sd_gui_utils::networks::PackaetParam param;
            size_t size;
            std::string data;
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Packet, type, size, data)
    }
}

#endif