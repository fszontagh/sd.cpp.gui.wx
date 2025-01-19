#ifndef _SDGUI_LIBS_NETWORK_PACKETS_H_
#define _SDGUI_LIBS_NETWORK_PACKETS_H_

namespace sd_gui_utils {
    inline namespace networks {

        class Packet {
        private:
            std::vector<unsigned char> data;
            size_t data_size = 0;

        public:
            enum class Type : int {
                REQUEST_TYPE,
                RESPONSE_TYPE,
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
                PARAM_JOB_DELETE
            };

            Packet(Type type, Param param)
                : type(type), param(param) {}
            Packet()
                : type(sd_gui_utils::networks::Packet::Type::REQUEST_TYPE), param(sd_gui_utils::networks::Packet::Param::PARAM_ERROR) {}
            sd_gui_utils::networks::Packet::Type type = sd_gui_utils::networks::Packet::Type::INVALID_TYPE;
            sd_gui_utils::networks::Packet::Param param;
            // std::string version   = std::string(SD_GUI_VERSION);
            std::string version     = std::string(SD_GUI_VERSION);
            int source_idx          = -1;
            int target_idx          = -1;
            std::string server_id   = "";
            std::string server_name = "";
            uint64_t client_id      = 0;
            bool isValid() {
                return this->type == sd_gui_utils::networks::Packet::Type::INVALID_TYPE ? false : true;
            }

            inline size_t GetSize() { return this->data_size; }
            template <typename T>
            inline void SetData(const T& j) {
                try {
                    const nlohmann::json json_obj = j;
                    std::string json_str    = json_obj.dump();
                    this->data              = std::vector<uint8_t>(json_str.begin(), json_str.end());
                    this->data_size         = this->data.size();
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to serialize object in SetData: " + std::string(e.what()));
                }
            }

            template <typename T>
            inline T GetData() {
                try {
                    nlohmann::json json_obj = nlohmann::json::parse(std::string(this->data.begin(), this->data.end()));
                    return json_obj.get<T>();
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to deserialize GetData data: " + std::string(e.what()));
                }
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
            NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Packet, type, param, version, data_size, data, server_id, server_name, source_idx, target_idx, client_id)
        };  // struct Packet
    }  // namespace networks
}  // namespace sd_gui_utils

#endif  // _SDGUI_LIBS_NETWORK_PACKETS_H_