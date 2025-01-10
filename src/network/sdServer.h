#ifndef _NETWORK_SD_SERVER_H_
#define _NETWORK_SD_SERVER_H_
#include "../libs/TcpClient.h"

namespace sd_gui_utils {

    class sdServer : public wxClientData {
        std::string name;  // name is get from the over tcp
        std::string host;
        bool authkey = false;  // auth key is set in the
        int port     = 0;
        std::atomic<bool> enabled{false};
        std::atomic<bool> needToRun{false};
        std::string server_id = "";
        int internal_id       = -1;
        std::string disconnect_reason;
        std::thread thread;
        std::unique_ptr<sd_gui_utils::networks::TcpClient> client = nullptr;
        wxEvtHandler* evt                                         = nullptr;
        bool IsOk() const { return !host.empty() && port > 0; }

    public:
        /// @brief Get the name of the server
        /// @details If the server name is empty, it returns the host and port concatenated with a colon, otherwise it returns the name given by the server.
        wxString GetName() const {
            if (this->name.empty()) {
                return wxString::Format("%s:%d", host.c_str(), port);
            }
            return wxString::FromUTF8Unchecked(this->name);
        }
        wxString GetsecretKeyName() const {
            return wxString::Format(wxT("%s/%s_%d"), PROJECT_NAME, wxString::FromUTF8Unchecked(this->host), this->port);
        }
        int GetInternalId() {
            return this->internal_id;
        }
        std::string GetId() {
            return this->server_id;
        }
        wxString GetDisconnectReason() {
            // after disconnect, the client will be nullptr
            return wxString::FromUTF8Unchecked(this->disconnect_reason);
        }
        bool IsEnabled() const { return this->enabled.load(); }
        void SetAuthKeyState(bool state) { this->authkey = state; }
        bool GetAuthKeyState() const { return this->authkey; }
        sd_gui_utils::networks::Packet GetPacket(size_t packet_id) {
            return this->client == nullptr ? sd_gui_utils::networks::Packet() : this->client->getPacket(packet_id);
        }
        void Stop() {
            this->needToRun.store(false);
            if (this->client != nullptr) {
                this->client->stop();
            }
        }

        /// @brief Get the status of the server
        /// @return "connected" if enabled and connected, "disconnected" if enabled but not connected, "disabled" if not enabled
        wxString GetStatus() const {
            if (this->client != nullptr) {
                return this->enabled.load() == true ? this->client->IsConnected() == true ? _("connected") : _("disconnected") : _("disabled");
            }
            return "disabled";
        };

        void SetEnabled(bool state = true) {
            this->enabled.store(state);
            if (state == false) {
                this->Stop();
            }
        }
        std::string GetHost() {
            return this->host;
        }
        void SetHost(const std::string& host) {
            this->host = host;
        }
        int GetPort() {
            return this->port;
        }
        void SetName(const std::string& name) {
            this->name = name;
        }
        void SetPort(int port) {
            this->port = port;
        }
        void SetId(const std::string& id) {
            this->server_id = id;
        }

        void SetInternalId(int id) {
            this->internal_id = id;
        }
        void StartServer() {
            this->needToRun.store(true);
            if (this->client == nullptr) {
                this->client = std::make_unique<sd_gui_utils::networks::TcpClient>();
            }
            this->thread = std::thread([this]() {
                while (this->needToRun.load()) {
                    int tries = 1;
                    while (this->client->IsConnected() == false) {
                        this->client->Connect(this->host, this->port);
                        if (this->client->IsConnected() || this->needToRun.load() == false) {
                            break;
                        }

                        // if (tries > 5 || this->needToRun.load() == false) {
                        //     this->needToRun.store(false);
                        //     this->client = nullptr;
                        //     break;
                        //}
                        tries++;
                        std::this_thread::sleep_for(std::chrono::seconds(5 * tries));
                        if (tries > 10) {
                            tries = 1;
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                if (this->client != nullptr) {
                    this->client->stop();
                }
                this->client = nullptr;
            });
        }
        sdServer(sdServer&& other) noexcept
            : name(std::move(other.name)),
              host(std::move(other.host)),
              authkey(other.authkey),
              port(other.port),
              enabled(other.enabled.load()),
              needToRun(other.needToRun.load()),
              server_id(std::move(other.server_id)),
              internal_id(other.internal_id),
              disconnect_reason(std::move(other.disconnect_reason)),
              thread(std::move(other.thread)),
              client(std::move(other.client)),
              evt(other.evt) {}

        sdServer& operator=(sdServer&& other) noexcept {
            if (this != &other) {
                Stop();
                name    = std::move(other.name);
                host    = std::move(other.host);
                authkey = other.authkey;
                port    = other.port;
                enabled.store(other.enabled.load());
                needToRun.store(other.needToRun.load());
                server_id         = std::move(other.server_id);
                internal_id       = other.internal_id;
                disconnect_reason = std::move(other.disconnect_reason);
                thread            = std::move(other.thread);
                client            = std::move(other.client);
                evt               = other.evt;
            }
            return *this;
        }
        sdServer(const std::string& host, int port, wxEvtHandler* evt)
            : host(host), port(port), evt(evt) {
            this->client               = std::make_unique<sd_gui_utils::networks::TcpClient>();
            this->client->onConnectClb = [this]() {
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_CONNECTED, this);
            };
            this->client->onMessageClb = [this](int packet_id) {
                auto msg = this->client->getPacket(packet_id);
                if (msg.server_id.empty() == false && this->server_id.empty()) {
                    this->SetId(msg.server_id);
                }
                if (msg.param == sd_gui_utils::networks::PacketParam::MODEL_LIST) {
                    this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_MODEL_LIST_UPDATE, this);
                }
            };
            this->client->onDisconnectClb = [this](const std::string& reason) {
                this->disconnect_reason = reason;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this);
            };
            this->client->onErrorClb = [this](std::string msg) {
                this->disconnect_reason = msg;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_ERROR, this);
            };
        }
        // copy constructor
        sdServer(const sdServer& other) = delete;

        sdServer& operator=(const sdServer&) = delete;
        sdServer()                           = default;
        ~sdServer() {
            Stop();
            if (thread.joinable()) {
                thread.join();
            }
        }
        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text = "") {
            wxThreadEvent* e = new wxThreadEvent();
            e->SetString(wxString::Format("%d:%s", (int)eventType, text));
            e->SetPayload(payload);
            wxQueueEvent(this->evt, e);
        };

        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, size_t packet_id, std::string text = "") {
            wxThreadEvent* e = new wxThreadEvent();
            e->SetString(wxString::Format("%d:%s", (int)eventType, text));
            e->SetInt(packet_id);
            e->SetPayload(payload);
            wxQueueEvent(this->evt, e);
        };
        inline void to_json(nlohmann ::json& nlohmann_json_j, const sdServer& nlohmann_json_t) {
            nlohmann_json_j["name"]        = nlohmann_json_t.name;
            nlohmann_json_j["host"]        = nlohmann_json_t.host;
            nlohmann_json_j["authkey"]     = nlohmann_json_t.authkey;
            nlohmann_json_j["port"]        = nlohmann_json_t.port;
            nlohmann_json_j["server_id"]   = nlohmann_json_t.server_id;
            nlohmann_json_j["internal_id"] = nlohmann_json_t.internal_id;
            nlohmann_json_j["enabled"]     = nlohmann_json_t.enabled.load();
        }
        inline void from_json(const nlohmann ::json& nlohmann_json_j, sdServer& nlohmann_json_t) {
            const sdServer nlohmann_json_default_obj{};
            nlohmann_json_t.name        = nlohmann_json_j.value("name", nlohmann_json_default_obj.name);
            nlohmann_json_t.host        = nlohmann_json_j.value("host", nlohmann_json_default_obj.host);
            nlohmann_json_t.authkey     = nlohmann_json_j.value("authkey", nlohmann_json_default_obj.authkey);
            nlohmann_json_t.port        = nlohmann_json_j.value("port", nlohmann_json_default_obj.port);
            nlohmann_json_t.server_id   = nlohmann_json_j.value("server_id", nlohmann_json_default_obj.server_id);
            nlohmann_json_t.internal_id = nlohmann_json_j.value("internal_id", nlohmann_json_default_obj.internal_id);
            nlohmann_json_t.enabled.store(nlohmann_json_j.value("enabled", nlohmann_json_default_obj.enabled.load()));
        }
    };
};  // namespace sd_gui_utils
#endif  // _NETWORK_SD_SERVER_H_