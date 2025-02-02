#ifndef _SDGUI_LIBS_NETWORK_PACKETS_H_
#define _SDGUI_LIBS_NETWORK_PACKETS_H_

namespace sd_gui_utils {
    inline namespace networks {

        // 8 bytes length to store the expected packet size (on x64 sizeof(size_t) = 8)
        inline const int PACKET_SIZE_LENGTH = 8;
        // 64KB buffer
        inline const int TCP_RX_BUFFER_SIZE = 64 * 1024;
        inline const int TCP_TX_BUFFER_SIZE = 64 * 1024;

        class Packet {
        private:
            std::vector<unsigned char> data;
            size_t data_size = 0;

        public:
            enum class Type : int {
                REQUEST_TYPE,
                RESPONSE_TYPE,
                DELETE_TYPE,
                INVALID_TYPE
            };

            enum class Param : int {
                PARAM_ERROR,
                PARAM_AUTH,
                PARAM_MODEL_LIST,
                PARAM_KEEPALIVE,
                PARAM_JOB_LIST,
                PARAM_JOB_ADD,
                PARAM_JOB_UPDATE,
                PARAM_JOB_DELETE,
                PARAM_JOB_IMAGE_LIST,
                PARAM_JOB_DUPLICATE,
                PARAM_JOB_RESUME,
                PARAM_JOB_PAUSE
            };

            Packet(Type type, Param param)
                : type(type), param(param) {}
            Packet()
                : type(sd_gui_utils::networks::Packet::Type::REQUEST_TYPE), param(sd_gui_utils::networks::Packet::Param::PARAM_ERROR) {}
            sd_gui_utils::networks::Packet::Type type = sd_gui_utils::networks::Packet::Type::INVALID_TYPE;
            sd_gui_utils::networks::Packet::Param param;
            // std::string version   = std::string(SD_GUI_VERSION);
            std::string version                    = std::string(SD_GUI_VERSION);
            std::string application_system_name    = std::string(SD_GUI_SYSTEM_NAME);
            std::string application_arch           = std::string(SD_GUI_ARCH);
            std::string application_system_version = std::string(SD_GUI_SYSTEM_VERSION);
            int source_idx                         = -1;
            int target_idx                         = -1;
            std::string server_id                  = "";
            std::string server_name                = "";
            uint64_t client_session_id             = 0;
            long packet_added_time                 = 0;
            bool isValid() {
                return this->type == sd_gui_utils::networks::Packet::Type::INVALID_TYPE ? false : true;
            }

            inline size_t GetSize() { return this->data_size; }
            template <typename T>
            inline bool SetData(const T& j) {
                try {
                    const nlohmann::json json_obj = j;
                    std::string json_str          = json_obj.dump();
                    this->data                    = std::vector<uint8_t>(json_str.begin(), json_str.end());
                    this->data_size               = this->data.size();
                    return true;
                } catch (const std::exception& e) {
                    std::cerr << "Failed to serialize object in SetData: " + std::string(e.what()) << std::endl;
                    return false;
                }
            }

            template <typename T>
            inline T GetData() {
                try {
                    nlohmann::json json_obj = nlohmann::json::parse(std::string(this->data.begin(), this->data.end()));
                    return json_obj.get<T>();
                } catch (const std::exception& e) {
                    std::cerr << "Failed to deserialize GetData data: " + std::string(e.what()) << std::endl;
                }
                return T();
            }

            size_t GetDataSize() { return this->data_size; }

            static std::pair<const char*, size_t> Serialize(const Packet& packet) {
                try {
                    nlohmann::json json_data = packet;
                    auto binary_data         = nlohmann::json::to_ubjson(json_data);
                    char* d                  = new char[binary_data.size() + 1];
                    std::copy(binary_data.begin(), binary_data.end(), d);
                    return std::make_pair(d, binary_data.size());
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to serialize Packet to MessagePack: " + std::string(e.what()));
                }
            }

            static Packet DeSerialize(const char* data, size_t size) {
                try {
                    std::vector<std::uint8_t> binary_data(data, data + size);
                    auto json_data = nlohmann::json::from_ubjson(binary_data);

                    if (json_data.is_null()) {
                        throw std::runtime_error("Received empty MessagePack data.");
                    }

                    return json_data.get<Packet>();
                } catch (const std::exception& e) {
                    std::string received_data(data, data + size);
                    throw std::runtime_error(
                        "Failed to convert Packet: " + std::string(e.what()) + "\nRaw data size: " + std::to_string(size));
                }
            }
            NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Packet, type, param, version, data_size, data, server_id, server_name, source_idx, target_idx, client_session_id)
        };  // struct Packet

        static const std::unordered_map<sd_gui_utils::networks::Packet::Param, std::string> PacketParam2str = {
            {sd_gui_utils::networks::Packet::Param::PARAM_ERROR, "ERROR"},
            {sd_gui_utils::networks::Packet::Param::PARAM_AUTH, "AUTH"},
            {sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST, "MODEL_LIST"},
            {sd_gui_utils::networks::Packet::Param::PARAM_KEEPALIVE, "KEEPALIVE"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_LIST, "JOB_LIST"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_ADD, "JOB_ADD"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_UPDATE, "JOB_UPDATE"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_DELETE, "JOB_DELETE"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_IMAGE_LIST, "JOB_IMAGE_LIST"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_DUPLICATE, "JOB_DUPLICATE"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_RESUME, "JOB_RESUME"},
            {sd_gui_utils::networks::Packet::Param::PARAM_JOB_PAUSE, "JOB_PAUSE"}};

        static const std::unordered_map<std::string, sd_gui_utils::networks::Packet::Param> str2PacketParam = {
            {"ERROR", sd_gui_utils::networks::Packet::Param::PARAM_ERROR},
            {"AUTH", sd_gui_utils::networks::Packet::Param::PARAM_AUTH},
            {"MODEL_LIST", sd_gui_utils::networks::Packet::Param::PARAM_MODEL_LIST},
            {"KEEPALIVE", sd_gui_utils::networks::Packet::Param::PARAM_KEEPALIVE},
            {"JOB_LIST", sd_gui_utils::networks::Packet::Param::PARAM_JOB_LIST},
            {"JOB_ADD", sd_gui_utils::networks::Packet::Param::PARAM_JOB_ADD},
            {"JOB_UPDATE", sd_gui_utils::networks::Packet::Param::PARAM_JOB_UPDATE},
            {"JOB_DELETE", sd_gui_utils::networks::Packet::Param::PARAM_JOB_DELETE},
            {"JOB_IMAGE_LIST", sd_gui_utils::networks::Packet::Param::PARAM_JOB_IMAGE_LIST},
            {"JOB_DUPLICATE", sd_gui_utils::networks::Packet::Param::PARAM_JOB_DUPLICATE},
            {"JOB_RESUME", sd_gui_utils::networks::Packet::Param::PARAM_JOB_RESUME},
            {"JOB_PAUSE", sd_gui_utils::networks::Packet::Param::PARAM_JOB_PAUSE}};

        static const std::unordered_map<sd_gui_utils::networks::Packet::Type, std::string> PacketType2str = {
            {sd_gui_utils::networks::Packet::Type::REQUEST_TYPE, "REQUEST"},
            {sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE, "RESPONSE"},
            {sd_gui_utils::networks::Packet::Type::DELETE_TYPE, "DELETE"},
            {sd_gui_utils::networks::Packet::Type::INVALID_TYPE, "INVALID"}};

        static const std::unordered_map<std::string, sd_gui_utils::networks::Packet::Type> str2PacketType = {
            {"REQUEST", sd_gui_utils::networks::Packet::Type::REQUEST_TYPE},
            {"RESPONSE", sd_gui_utils::networks::Packet::Type::RESPONSE_TYPE},
            {"DELETE", sd_gui_utils::networks::Packet::Type::DELETE_TYPE},
            {"INVALID", sd_gui_utils::networks::Packet::Type::INVALID_TYPE}};

        struct DeleteResponse {
            uint64_t job_id = 0;
            bool state;
            std::string error = "";
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DeleteResponse, job_id, state, error)

        struct ImageRequest {
            uint64_t job_id       = 0;
            std::string image_id = {};
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ImageRequest, job_id, image_id)

        struct ImageResponse {
            ImageInfo image_info;
        };

        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ImageResponse, image_info)

    }  // namespace networks
}  // namespace sd_gui_utils

#endif  // _SDGUI_LIBS_NETWORK_PACKETS_H_