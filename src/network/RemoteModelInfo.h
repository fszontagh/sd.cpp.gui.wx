#ifndef _NETWORK_REMOTE_MODEL_INFO_H_
#define _NETWORK_REMOTE_MODEL_INFO_H_

#ifndef _HELPERS_DIR_TYPES_H_
#include "helpers/DirTypes.h"
#endif

namespace sd_gui_utils {
    inline namespace networks {

        class RemoteModelInfo {
        public:
            int server_id = -1;  // deleted server id
            std::string name;
            std::string path;
            std::string root_path;
            std::string sha256;
            size_t size;
            std::string size_f;
            size_t hash_progress_size;
            size_t hash_fullsize;
            sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;

            RemoteModelInfo() = default;
            RemoteModelInfo(const wxFileName& path, sd_gui_utils::DirTypes type, const wxString& root_path) {
                this->model_type         = type;
                this->name               = path.GetFullName();
                this->size               = path.GetSize().GetValue();
                this->size_f             = path.GetHumanReadableSize().utf8_string();
                this->hash_fullsize      = 0;
                this->hash_progress_size = 0;
                this->path               = path.GetAbsolutePath().utf8_string();
                this->root_path          = root_path.utf8_string();
            }
            ~RemoteModelInfo() {}
            friend void to_json(nlohmann ::json& nlohmann_json_j, const RemoteModelInfo& nlohmann_json_t) {
                nlohmann_json_j["server_id"]          = nlohmann_json_t.server_id;
                nlohmann_json_j["name"]               = nlohmann_json_t.name;
                nlohmann_json_j["path"]               = nlohmann_json_t.path;
                nlohmann_json_j["root_path"]          = nlohmann_json_t.root_path;
                nlohmann_json_j["sha256"]             = nlohmann_json_t.sha256;
                nlohmann_json_j["size"]               = nlohmann_json_t.size;
                nlohmann_json_j["size_f"]             = nlohmann_json_t.size_f;
                nlohmann_json_j["hash_progress_size"] = nlohmann_json_t.hash_progress_size;
                nlohmann_json_j["hash_fullsize"]      = nlohmann_json_t.hash_fullsize;
                nlohmann_json_j["model_type"]         = nlohmann_json_t.model_type;
            }
            friend void from_json(const nlohmann ::json& nlohmann_json_j, RemoteModelInfo& nlohmann_json_t) {
                {
                    auto iter = nlohmann_json_j.find("server_id");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.server_id);
                }
                {
                    auto iter = nlohmann_json_j.find("name");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.name);
                }
                {
                    auto iter = nlohmann_json_j.find("path");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.path);
                }
                {
                    auto iter = nlohmann_json_j.find("root_path");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.root_path);
                }
                {
                    auto iter = nlohmann_json_j.find("sha256");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.sha256);
                }
                {
                    auto iter = nlohmann_json_j.find("size");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.size);
                }
                {
                    auto iter = nlohmann_json_j.find("size_f");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.size_f);
                }
                {
                    auto iter = nlohmann_json_j.find("hash_progress_size");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.hash_progress_size);
                }
                {
                    auto iter = nlohmann_json_j.find("hash_fullsize");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.hash_fullsize);
                }
                {
                    auto iter = nlohmann_json_j.find("model_type");
                    if (iter != nlohmann_json_j.end())
                        if (!iter->is_null())
                            iter->get_to(nlohmann_json_t.model_type);
                }
            }
        };

        typedef std::vector<RemoteModelInfo> RemoteModelList;
    }
}
#endif  // _NETWORK_REMOTE_MODEL_INFO_H_