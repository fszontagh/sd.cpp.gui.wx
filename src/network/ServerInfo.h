#ifndef _SDGUI_NETWORKS_SERVERINFO_H_
#define _SDGUI_NETWORKS_SERVERINFO_H_

namespace sd_gui_utils {
    inline namespace networks {
        struct ServerInfo {
            unsigned int cpu_cores;
            std::string server_name;
            std::string server_id = "";
        }
    };
}

#endif  // _SDGUI_NETWORKS_SERVERINFO_H_