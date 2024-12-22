#ifndef _NETWORK_REMOTE_MODEL_INFO_H_
#define _NETWORK_REMOTE_MODEL_INFO_H_

#ifndef _HELPERS_DIR_TYPES_H_
#include "helpers/DirTypes.h"
#endif

namespace sd_gui_utils::networks {

    class RemoteModelInfo {
    public:
        int server_id = -1;  // deleted server id
        std::string name;
        std::string path;
        std::string sha256;
        size_t size;
        std::string size_f;
        size_t hash_progress_size;
        size_t hash_fullsize;
        sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;

        RemoteModelInfo() = default;
        RemoteModelInfo(const wxFileName &path, sd_gui_utils::DirTypes type) {
            this->model_type = type;
            this->name = path.GetFullName();
            this->size = path.GetSize().GetValue();
            this->size_f = path.GetHumanReadableSize().ToStdString();
            this->hash_fullsize = 0;
            this->hash_progress_size = 0;
            this->path = path.GetAbsolutePath().ToStdString();
        }
        ~RemoteModelInfo() {}
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(RemoteModelInfo, server_id, name, path, sha256, size, size_f, hash_progress_size, hash_fullsize, model_type)
    };

    typedef std::vector<RemoteModelInfo> RemoteModelList;
}  // namespace sd_gui_utils::networks
#endif  // _NETWORK_REMOTE_MODEL_INFO_H_