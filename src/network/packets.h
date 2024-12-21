#ifndef _NETWORK_PACKETS_H_
#define _NETWORK_PACKETS_H_

namespace sd_gui_utils {
    namespace networks {

        enum class PacketType {
            REQUEST,
            RESPONSE
        };
        enum class PacketParam {
            ERROR,
            AUTH,
            MODEL_LIST
        };

        struct Packet {
            Packet(sd_gui_utils::networks::PacketType type, sd_gui_utils::networks::PacketParam param, const std::string& data = "")
                : type(type), param(param), data(data), size(data.size()) {}
            Packet() = default;
            sd_gui_utils::networks::PacketType type;
            sd_gui_utils::networks::PacketParam param;
            size_t size;
            std::string data;
            std::string version = std::string(SD_GUI_VERSION);
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Packet, type, param, size, data, version)
    }
}

#endif