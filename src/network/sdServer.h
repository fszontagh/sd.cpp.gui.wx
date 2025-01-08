#ifndef _NETWORK_SD_SERVER_H_
#define _NETWORK_SD_SERVER_H_

namespace sd_gui_utils {
    namespace networks {
        class TcpClient;
    }

    struct sdServer : public wxClientData {
        std::string name;  // name is get from the over tcp
        std::string host;
        bool authkey                = false;  // auth key is set in the secret
        int port                    = 0;
        std::atomic<bool> connected = false;  // we check if the server is connected and not needed to store it into config
        std::atomic<bool> enabled   = {false};
        int server_id               = -1;
        std::atomic<bool> needToRun = {false};
        std::string disconnect_reason;
        std::shared_ptr<sd_gui_utils::networks::TcpClient> client = nullptr;
        bool IsOk() const { return !host.empty() && port > 0; }

        /// @brief Get the name of the server
        /// @details If the server name is empty, it returns the host and port concatenated with a colon, otherwise it returns the name given by the server.
        wxString GetName() const {
            if (this->name.empty()) {
                return wxString::Format("%s:%d", host.c_str(), port);
            }
            return wxString::FromUTF8Unchecked(this->name);
        }

        /// @brief Get the status of the server
        /// @return "connected" if enabled and connected, "disconnected" if enabled but not connected, "disabled" if not enabled
        wxString GetStatus() const {
            return this->enabled ? this->connected ? "connected" : "disconnected" : "disabled";
        };
        sdServer& operator=(const sdServer&) = default;
        sdServer(const std::string& host, int port)
            : host(host), port(port) {}
        // copy constructor
        sdServer(const sdServer& other) {
            this->name      = other.name;
            this->host      = other.host;
            this->authkey   = other.authkey;
            this->port      = other.port;
            this->connected = other.connected.load();
            this->server_id = other.server_id;
            this->enabled   = other.enabled.load();
        };
        sdServer() {}
    };
    inline void to_json(nlohmann ::json& nlohmann_json_j, const sdServer& nlohmann_json_t) {
        nlohmann_json_j["name"]      = nlohmann_json_t.name;
        nlohmann_json_j["host"]      = nlohmann_json_t.host;
        nlohmann_json_j["authkey"]   = nlohmann_json_t.authkey;
        nlohmann_json_j["port"]      = nlohmann_json_t.port;
        nlohmann_json_j["connected"] = nlohmann_json_t.connected.load();
        nlohmann_json_j["server_id"] = nlohmann_json_t.server_id;
        nlohmann_json_j["enabled"]   = nlohmann_json_t.enabled.load();
    }
    inline void from_json(const nlohmann ::json& nlohmann_json_j, sdServer& nlohmann_json_t) {
        const sdServer nlohmann_json_default_obj{};
        nlohmann_json_t.name      = nlohmann_json_j.value("name", nlohmann_json_default_obj.name);
        nlohmann_json_t.host      = nlohmann_json_j.value("host", nlohmann_json_default_obj.host);
        nlohmann_json_t.authkey   = nlohmann_json_j.value("authkey", nlohmann_json_default_obj.authkey);
        nlohmann_json_t.port      = nlohmann_json_j.value("port", nlohmann_json_default_obj.port);
        nlohmann_json_t.connected = nlohmann_json_j.value("connected", false);
        nlohmann_json_t.server_id = nlohmann_json_j.value("server_id", nlohmann_json_default_obj.server_id);
        nlohmann_json_t.enabled   = nlohmann_json_j.value("enabled", false);
    }
}  // namespace sd_gui_utils
#endif  // _NETWORK_SD_SERVER_H_