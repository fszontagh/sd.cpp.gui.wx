#ifndef _NETWORK_PACKETS_H_
#define _NETWORK_PACKETS_H_

namespace sd_gui_utils {

    static std::string toHex(const char* data, size_t size) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');

        for (size_t i = 0; i < size; ++i) {
            ss << std::setw(2) << static_cast<int>(data[i] & 0xFF);
        }

        return ss.str();
    }

    namespace networks {

        enum class PacketType {
            REQUEST  = 0,
            RESPONSE = 1
        };

        NLOHMANN_JSON_SERIALIZE_ENUM(PacketType, {
                                                     {PacketType::REQUEST, 0},
                                                     {PacketType::RESPONSE, 1},
                                                 });

        enum class PacketParam {
            ERROR      = 0,
            AUTH       = 1,
            MODEL_LIST = 2,
            KEEPALIVE = 3
        };

        NLOHMANN_JSON_SERIALIZE_ENUM(PacketParam, {{PacketParam::ERROR, 0}, {PacketParam::AUTH, 1}, {PacketParam::MODEL_LIST, 2}})

        struct Packet {
        private:
            std::vector<uint8_t> data;
            size_t data_size = 0;

        public:
            Packet(sd_gui_utils::networks::PacketType type, sd_gui_utils::networks::PacketParam param)
                : type(type), param(param) {}
            Packet()
                : type(PacketType::REQUEST), param(PacketParam::ERROR) {}
            sd_gui_utils::networks::PacketType type;
            sd_gui_utils::networks::PacketParam param;
            std::string version = std::string(SD_GUI_VERSION);
            int source_idx      = -1;
            int target_idx      = -1;

            inline size_t GetSize() { return this->data_size; }
            template <typename T>
            void SetData(const T& j) {
                try {
                    nlohmann::json json_obj = j;
                    std::string json_str    = json_obj.dump();
                    this->data              = std::vector<uint8_t>(json_str.begin(), json_str.end());
                    this->data_size         = this->data.size();
                } catch (const std::exception& e) {
                    throw std::runtime_error("Failed to serialize object in SetData: " + std::string(e.what()));
                }
            }

            template <typename T>
            T GetData() {
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

                    // Ellenőrizzük, hogy valóban megfelelő adat érkezett
                    if (json_data.is_null()) {
                        throw std::runtime_error("Received empty MessagePack data.");
                    }

                    return json_data.get<Packet>();
                } catch (const std::exception& e) {
                    // Debugging kiírás hexadecimálisan
                    std::string received_data(data, data + size);
                    throw std::runtime_error(
                        "Failed to convert MessagePack to Packet: " + std::string(e.what()) + "\nRaw data size: " + std::to_string(size) + "\nRaw data: " + sd_gui_utils::toHex(data, size));
                }
            }

            friend void to_json(nlohmann ::json& nlohmann_json_j, const Packet& nlohmann_json_t) {
                nlohmann_json_j["type"]      = (int)nlohmann_json_t.type;
                nlohmann_json_j["param"]     = (int)nlohmann_json_t.param;
                nlohmann_json_j["version"]   = nlohmann_json_t.version;
                nlohmann_json_j["data_size"] = nlohmann_json_t.data_size;
                nlohmann_json_j["data"]      = nlohmann_json_t.data;
            }
            friend void from_json(const nlohmann ::json& nlohmann_json_j, Packet& nlohmann_json_t) {
                const Packet nlohmann_json_default_obj{};
                nlohmann_json_t.type      = (sd_gui_utils::networks::PacketType)nlohmann_json_j.value("type", sd_gui_utils::networks::PacketType::REQUEST);
                nlohmann_json_t.param     = (sd_gui_utils::networks::PacketParam)nlohmann_json_j.value("param", sd_gui_utils::networks::PacketParam::ERROR);
                nlohmann_json_t.version   = nlohmann_json_j.value("version", nlohmann_json_default_obj.version);
                nlohmann_json_t.data_size = nlohmann_json_j.value("data_size", nlohmann_json_default_obj.data_size);
                nlohmann_json_t.data      = nlohmann_json_j.value("data", nlohmann_json_default_obj.data);
            }
        };
    }  // namespace networks
}  // namespace sd_gui_utils

#endif