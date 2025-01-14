#ifndef _NETWORK_SD_SERVER_H_
#define _NETWORK_SD_SERVER_H_
#include "../libs/TcpClient.h"

namespace sd_gui_utils {

    class sdServer : public wxClientData {
        std::string name = "";  // name is get from the over tcp
        std::string host = "";
        bool authkey     = false;  // auth key is set in the
        int port         = 0;
        std::atomic<bool> enabled{false};
        std::atomic<bool> needToRun{false};
        std::string server_id = "";
        int internal_id       = -1;
        uint64_t client_id    = 0;
        std::string disconnect_reason;
        std::thread thread;
        std::shared_ptr<sd_gui_utils::networks::TcpClient> client = nullptr;
        wxEvtHandler* evt                                         = nullptr;
        bool IsOk() const { return !host.empty() && port > 0; }
        mutable std::mutex mutex;
        void initCallbacks() {
            this->client->SetOnAuthRequestCallback([this]() {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onAuthRequestClb" << std::endl;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_AUTH_REQUEST, this);
            });

            this->client->SetOnConnectCallback([this]() {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onConnectClb" << std::endl;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_CONNECTED, this);
            });

            this->client->SetOnMessageCallback([this](int packet_id) {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onMessageClb" << std::endl;
                auto msg = this->client->getPacket(packet_id);
                if (msg.isValid() == false) {
                    return;
                }
                if (msg.param == sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST) {
                    this->SetId(msg.server_id);  // update the server's id
                    this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_MODEL_LIST_UPDATE, this, packet_id);
                    return;
                }
                if (msg.param == sd_gui_utils::Packet::Param::PARAM_AUTH) {
                    // std::string data = msg.GetData<std::string>();
                    this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_AUTH_RESPONSE, this, packet_id);
                    return;
                }
            });

            this->client->SetOnDisconnectCallback([this](const std::string& reason) {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onDisconnectClb" << std::endl;
                this->disconnect_reason = reason;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED, this, reason);
            });

            this->client->SetOnErrorCallback([this](std::string msg) {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onErrorClb" << std::endl;
                this->disconnect_reason = msg;
                this->SendThreadEvent(sd_gui_utils::ThreadEvents::SERVER_ERROR, this, msg);
            });

            this->client->SetOnStopCallback([this]() {
                // std::lock_guard<std::mutex> lock(this->mutex);
                std::cout << "sdServer: onStopClb" << std::endl;
                // this->needToRun.store(false);
            });
        }

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
        uint64_t GetClientId() {
            return this->client_id;
        }
        void SetClientId(uint64_t id) {
            if (id == 0) {
                return;
            }
            this->client_id = id;
        }
        uint64_t GetClientIdFromPacket(int packet_id) {
            auto packet = this->GetPacket(packet_id);
            if (packet.isValid() == false) {
                return 0;
            }
            return packet.client_id;
        }
        wxString GetDisconnectReason() {
            // std::lock_guard<std::mutex> lock(this->mutex);
            return wxString::FromUTF8Unchecked(this->disconnect_reason);
        }
        bool IsEnabled() const { return this->enabled.load(); }
        void SetAuthKeyState(bool state) { this->authkey = state; }
        bool GetAuthKeyState() const { return this->authkey; }
        sd_gui_utils::networks::Packet GetPacket(size_t packet_id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            return this->client == nullptr ? sd_gui_utils::networks::Packet() : this->client->getPacket(packet_id);
        }
        void Stop() {
            this->needToRun.store(false);
            if (this->thread.joinable()) {
                this->thread.join();
            }
        }

        /// @brief Get the status of the server
        /// @return "connected" if enabled and connected, "disconnected" if enabled but not connected, "disabled" if not enabled
        wxString GetStatus() const {
            if (this->client != nullptr) {
                if (this->disconnect_reason.empty() == false) {
                    return this->disconnect_reason;
                }
                return this->enabled.load() == true ? this->client->IsConnected() == true ? _("connected") : _("disconnected") : _("disabled");
            }
            return "disabled";
        };

        /**
         * @brief Enable or disable the server.
         *
         * @param state Set to true to enable the server, false to disable.
         * @param autostartstop If true, automatically start or stop the server based on the state.
         */
        void SetEnabled(bool state = true, bool autostartstop = false) {
            this->enabled.store(state);
            if (state == false && autostartstop == true) {
                this->Stop();
            }
            if (state == true && autostartstop == true) {
                this->Stop();
                this->StartServer();
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
            std::cout << "sdServer: starting server" << std::endl;
            if (this->client->IsConnected() == true) {
                std::cout << "sdServer: server is already running" << std::endl;
                return;
            }

            this->needToRun.store(true);
            this->thread = std::thread([this]() {
                std::cout << "sdServer: starting thread" << std::endl;

                while (this->needToRun.load()) {
                    int tries = 1;

                    while (!this->client->IsConnected() && this->needToRun.load()) {
                        {
                            {
                                std::cout << "Connecting... " << this->host << ":" << this->port << std::endl;
                                std::lock_guard<std::mutex> lock(this->mutex);
                                this->client = nullptr;
                                this->client = std::make_shared<sd_gui_utils::networks::TcpClient>();
                                this->initCallbacks();
                                if (this->client->Connect(this->host, this->port) == true) {
                                    std::cout << "sdServer: connected" << std::endl;
                                    break;
                                }
                                this->client->stop();
                            }
                        }

                        tries++;
                        std::cout << "sdServer: trying to connect: " << tries << " sleeping 1s" << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(5));

                        if (tries > 10) {
                            tries = 1;
                        }
                    }
                    if (this->needToRun.load()) {
                        std::cout << "sdServer: sleep 10s" << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    }
                }

                std::cout << "sdServer: thread stopped" << std::endl;
            });

            std::cout << "sdServer: StartServer exiting" << std::endl;
        }

        void SendData(const std::string& data, Packet::Param param, Packet::Type type = Packet::Type::REQUEST_TYPE) {
            std::lock_guard<std::mutex> lock(this->mutex);
            Packet packet(Packet::Type::REQUEST_TYPE, Packet::Param::PARAM_AUTH);
            packet.SetData(data);
            packet.client_id = this->GetClientId();
            this->client->sendMsg(packet);
        }

        void SendPacket(sd_gui_utils::networks::Packet& packet) {
            std::lock_guard<std::mutex> lock(this->mutex);
            packet.client_id = this->GetClientId();
            this->client->sendMsg(packet);
        }

        void GetModelList() {
            std::lock_guard<std::mutex> lock(this->mutex);
            auto modelListPacket      = sd_gui_utils::networks::Packet();
            modelListPacket.type      = sd_gui_utils::networks::Packet::Type::REQUEST_TYPE;
            modelListPacket.param     = sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST;
            modelListPacket.client_id = this->GetClientId();
            this->client->sendMsg(modelListPacket);
        }
        bool IsConnected() {
            return this->client->IsConnected();
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
            this->client = std::make_shared<sd_gui_utils::networks::TcpClient>();

            this->initCallbacks();
        }
        // copy constructor
        sdServer(const sdServer& other) = delete;

        sdServer& operator=(const sdServer&) = delete;
        sdServer()                           = default;
        ~sdServer() {
            std::cout << "sdServer: destructor" << std::endl;
            this->Stop();
        }

        void SendAuthToken(const std::string token) {
            auto packet  = sd_gui_utils::networks::Packet();
            packet.type  = sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE;
            packet.param = sd_gui_utils::networks::Packet::Param::PARAM_AUTH;
            packet.SetData(token);
            packet.client_id = this->GetClientId();
            this->client->sendMsg(packet);
        }
        void RequestModelList() {
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            auto packet      = sd_gui_utils::networks::Packet();
            packet.type      = sd_gui_utils::networks::Packet::Type::REQUEST_TYPE;
            packet.param     = sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST;
            packet.client_id = this->GetClientId();
            this->client->sendMsg(packet);
        }
        void RequestJobList() {
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            auto packet      = sd_gui_utils::networks::Packet();
            packet.type      = sd_gui_utils::networks::Packet::Type::REQUEST_TYPE;
            packet.param     = sd_gui_utils::networks::Packet::Param::PARAM_JOBLIST;
            packet.client_id = this->GetClientId();
            this->client->sendMsg(packet);
        }
        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text = "") {
            if (!this->evt) {
                return;
            }
            wxThreadEvent* e = new wxThreadEvent();
            e->SetString(wxString::Format("%d:%s", (int)eventType, text));
            e->SetPayload(payload);
            wxQueueEvent(this->evt, e);
        };

        template <typename T>
        void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, size_t packet_id, std::string text = "") {
            if (!this->evt) {
                return;
            }
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
            nlohmann_json_j["client_id"]   = nlohmann_json_t.client_id;
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
            nlohmann_json_t.client_id   = nlohmann_json_j.value("client_id", nlohmann_json_default_obj.client_id);
            nlohmann_json_t.enabled.store(nlohmann_json_j.value("enabled", nlohmann_json_default_obj.enabled.load()));
        }
    };
};  // namespace sd_gui_utils
#endif  // _NETWORK_SD_SERVER_H_