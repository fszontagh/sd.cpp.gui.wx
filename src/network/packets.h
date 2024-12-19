#ifndef _NETWORK_PACKETS_H_
#define _NETWORK_PACKETS_H_
#include "libs/json.hpp"

namespace sd_gui_utils {
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