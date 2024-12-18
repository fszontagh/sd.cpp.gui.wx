#ifndef _NETWORK_PACKETS_H_
#define _NETWORK_PACKETS_H_
#include "libs/json.hpp"

namespace sd_gui_utils {
    namespace networks {

        enum class PacketType {
            Request = 0,
            Response
        };
        struct Packet {
            sd_gui_utils::networks::PacketType type;
            size_t size;
            char* data;
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Packet, type, size, data)
    }
}

#endif