#ifndef __MAINFRAME_HPP__UTILS
#define __MAINFRAME_HPP__UTILS

namespace sd_gui_utils {

    /**
     * \brief Splits a string into a vector of integers using a separator character.
     *
     * This function takes a string and a separator character, and splits the string
     * into a vector of integers using the separator character.
     *
     * \param str The string to be split.
     * \param sep The separator character to use.
     */
    inline std::vector<int> splitStr2int(const std::string& str, char sep) {
        std::vector<int> tokens;
        std::string token;
        for (char c : str) {
            if (c == sep) {
                if (!token.empty()) {
                    tokens.push_back(std::stoi(token));
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(std::stoi(token));
        }
        return tokens;
    }

    typedef struct VoidHolder {
        void* p1;  // eventhandler
        void* p2;  // QueueItem
        void* p3;  // Others...
    } VoidHolder;

    enum CivitAiState { OK,
                        NOT_FOUND,
                        ERR,
                        NOT_CHECKED };
    inline const char* civitai_state_str[] = {
        "Ok",
        "Not found",
        "Parse error",
        "",
    };

    class ModelFileInfo {
    public:
        std::string name;
        std::string path = "";
        std::string url;
        std::string poster;
        std::string sha256;
        sd_gui_utils::ModelInfoTag tags = sd_gui_utils::ModelInfoTag::None;
        size_t size;
        std::string size_f;
        std::string meta_file;
        size_t hash_progress_size         = 0;
        size_t hash_fullsize              = 0;
        sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;
        std::string civitaiPlainJson;
        CivitAi::ModelInfo CivitAiInfo;
        sd_gui_utils::CivitAiState state = sd_gui_utils::CivitAiState::NOT_CHECKED;
        std::vector<std::string> preview_images;
        std::string folderGroupName = "";
        std::string target_filename = "";
        int move_progress           = 0;
        std::string server_id       = "";

        ModelFileInfo() = default;
        ModelFileInfo(const sd_gui_utils::networks::RemoteModelInfo& remote) { *this = remote; }
        ModelFileInfo(const sd_gui_utils::ModelFileInfo& other)
            : name(other.name),
              path(other.path),
              url(other.url),
              poster(other.poster),
              sha256(other.sha256),
              tags(other.tags),
              size(other.size),
              size_f(other.size_f),
              meta_file(other.meta_file),
              hash_progress_size(other.hash_progress_size),
              hash_fullsize(other.hash_fullsize),
              model_type(other.model_type),
              civitaiPlainJson(other.civitaiPlainJson),
              CivitAiInfo(other.CivitAiInfo),
              state(other.state),
              preview_images(other.preview_images),
              folderGroupName(other.folderGroupName),
              server_id(other.server_id) {}
        ModelFileInfo& operator=(const sd_gui_utils::ModelFileInfo& other) {
            if (this != &other) {
                name               = other.name;
                path               = other.path;
                url                = other.url;
                poster             = other.poster;
                sha256             = other.sha256;
                tags               = other.tags;
                size               = other.size;
                size_f             = other.size_f;
                meta_file          = other.meta_file;
                hash_progress_size = other.hash_progress_size;
                hash_fullsize      = other.hash_fullsize;
                model_type         = other.model_type;
                civitaiPlainJson   = other.civitaiPlainJson;
                CivitAiInfo        = other.CivitAiInfo;
                state              = other.state;
                preview_images     = other.preview_images;
                folderGroupName    = other.folderGroupName;
                server_id          = other.server_id;
            }
            return *this;
        }
        ModelFileInfo& operator=(const sd_gui_utils::networks::RemoteModelInfo& remote) {
            name               = remote.name;
            path               = remote.path;
            sha256             = remote.sha256;
            size               = remote.size;
            size_f             = remote.size_f;
            hash_progress_size = remote.hash_progress_size;
            hash_fullsize      = remote.hash_fullsize;
            model_type         = remote.model_type;
            server_id          = remote.server_id;

            // Defaults for missing RemoteModelInfo fields
            url              = "";
            poster           = "";
            tags             = sd_gui_utils::ModelInfoTag::None;
            meta_file        = "";
            civitaiPlainJson = "";
            state            = sd_gui_utils::CivitAiState::NOT_CHECKED;
            preview_images.clear();
            folderGroupName = "";
            target_filename = "";
            move_progress   = 0;

            return *this;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo& rh) const {
            return (path == rh.path && server_id == rh.server_id);
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo rh) const {
            return (path == rh.path && server_id == rh.server_id);
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo* rh) const {
            if (rh == nullptr) {
                return false;
            }
            return (path == rh->path && server_id == rh->server_id);
        }
        inline ModelFileInfo operator+(const sd_gui_utils::networks::RemoteModelInfo& remote) const {
            ModelFileInfo merged      = *this;
            merged.name               = remote.name != merged.name ? remote.name : merged.name;
            merged.path               = remote.path != merged.path ? remote.path : merged.path;
            merged.sha256             = remote.sha256 != remote.sha256 ? remote.sha256 : merged.sha256;
            merged.size               = remote.size != merged.size ? remote.size : merged.size;
            merged.size_f             = remote.size_f != merged.size_f ? remote.size_f : merged.size_f;
            merged.hash_progress_size = remote.hash_progress_size != merged.hash_progress_size ? remote.hash_progress_size : merged.hash_progress_size;
            merged.hash_fullsize      = remote.hash_fullsize != merged.hash_fullsize ? remote.hash_fullsize : merged.hash_fullsize;
            merged.model_type         = remote.model_type != merged.model_type ? remote.model_type : merged.model_type;
            merged.server_id          = remote.server_id != merged.server_id ? remote.server_id : merged.server_id;
            return merged;
        }
        inline void operator+=(const sd_gui_utils::networks::RemoteModelInfo& remote) {
            if (remote.name != name) {
                name = remote.name;
            }
            if (remote.path != path) {
                path = remote.path;
            }
            if (remote.sha256 != sha256) {
                sha256 = remote.sha256;
            }
            if (remote.size != size) {
                size = remote.size;
            }
            if (remote.size_f != size_f) {
                size_f = remote.size_f;
            }
            if (remote.hash_progress_size != hash_progress_size) {
                hash_progress_size = remote.hash_progress_size;
            }
            if (remote.hash_fullsize != hash_fullsize) {
                hash_fullsize = remote.hash_fullsize;
            }
            if (remote.model_type != model_type) {
                model_type = remote.model_type;
            }
            if (remote.server_id != server_id) {
                server_id = remote.server_id;
            }
        }
    };

    inline void to_json(nlohmann::json& j, const sd_gui_utils::ModelFileInfo& p) {
        j = nlohmann::json{{"name", wxString(p.name.c_str()).utf8_string()},
                           {"path", wxString(p.path.c_str()).utf8_string()},
                           {"url", p.url},
                           {"poster", wxString(p.poster.c_str()).utf8_string()},
                           {"sha256", p.sha256},
                           {"tags", static_cast<int>(p.tags)},
                           {"size", p.size},
                           {"size_f", p.size_f},
                           {"meta_file", wxString(p.meta_file.c_str()).utf8_string()},
                           {"model_type", (int)p.model_type},
                           {"civitaiPlainJson", p.civitaiPlainJson},
                           {"CivitAiInfo", p.CivitAiInfo},
                           {"state", (int)p.state},
                           {"preview_images", p.preview_images},
                           {"folderGroupName", p.folderGroupName},
                           {"server_id", p.server_id}

        };
    }

    inline void from_json(const nlohmann::json& j, ModelFileInfo& p) {
        if (j.contains("name")) {
            p.name = wxString::FromUTF8(j.at("name").get<std::string>());
        }

        if (j.contains("path")) {
            p.path = wxString::FromUTF8(j.at("path").get<std::string>());
        }

        if (j.contains("url")) {
            j.at("url").get_to(p.url);
        }

        if (j.contains("poster")) {
            p.poster = wxString::FromUTF8(j.at("poster").get<std::string>());
        }

        if (j.contains("sha256")) {
            j.at("sha256").get_to(p.sha256);
        }

        if (j.contains("size")) {
            j.at("size").get_to(p.size);
        }

        if (j.contains("size_f")) {
            j.at("size_f").get_to(p.size_f);
        }

        if (j.contains("meta_file")) {
            p.meta_file = wxString::FromUTF8(j.at("meta_file").get<std::string>().c_str());
        }

        if (j.contains("model_type")) {
            p.model_type = j.at("model_type").get<sd_gui_utils::DirTypes>();
        }

        if (j.contains("civitaiPlainJson")) {
            p.civitaiPlainJson = j.at("civitaiPlainJson").get<std::string>();
        }

        if (j.contains("CivitAiInfo")) {
            p.CivitAiInfo = j.at("CivitAiInfo").get<CivitAi::ModelInfo>();
        }
        if (j.contains("state")) {
            p.state = j.at("state").get<sd_gui_utils::CivitAiState>();
        }
        if (j.contains("preview_images")) {
            p.preview_images = j.at("preview_images").get<std::vector<std::string>>();
        }
        if (j.contains("folderGroupName")) {
            p.folderGroupName = j.at("folderGroupName").get<std::string>();
        }
        if (j.contains("tags")) {
            p.tags = static_cast<sd_gui_utils::ModelInfoTag>(j.at("tags").get<int>());
        }
        if (j.contains("server_id")) {
            p.server_id = j.at("server_id").get<std::string>();
        }
    }
    enum imageTypes { JPG,
                      PNG,
                      WEBP };

    inline const std::unordered_map<imageTypes, wxString> image_types_str = {
        {sd_gui_utils::imageTypes::JPG, "JPG"},
        {sd_gui_utils::imageTypes::PNG, "PNG"}};

    inline const std::unordered_map<wxString, sd_gui_utils::imageTypes> image_types_str_reverse = {
        {"JPG", sd_gui_utils::imageTypes::JPG},
        {"PNG", sd_gui_utils::imageTypes::PNG}};

    inline wxArrayString GetSubdirectories(const wxString& folderPath) {
        wxArrayString directories;

        // Lambda function for recursive scanning
        std::function<void(const wxString&)> scanDirectory = [&](const wxString& path) {
            wxDir dir(path);
            if (!dir.IsOpened()) {
                return;  // Skip if the directory cannot be opened
            }

            wxString subdir;
            bool cont = dir.GetFirst(&subdir, wxEmptyString, wxDIR_DIRS);
            while (cont) {
                wxString fullPath = wxFileName(path, subdir).GetFullPath();
                directories.Add(fullPath);
                scanDirectory(fullPath);  // Recursively scan the subdirectory
                cont = dir.GetNext(&subdir);
            }
        };

        scanDirectory(folderPath);
        return directories;
    }

    class config {
    private:
        wxConfigBase* configBase = nullptr;
        std::mutex mutex;

    public:
        // store the servers in map: int -> internal_id
        std::map<int, sd_gui_utils::sdServer*> servers;
        wxString model                          = "";
        wxString vae                            = "";
        wxString lora                           = "";
        wxString embedding                      = "";
        wxString taesd                          = "";
        wxString esrgan                         = "";
        wxString presets                        = "";
        wxString prompt_templates               = "";
        wxString output                         = "";
        wxString jobs                           = "";
        wxString controlnet                     = "";
        wxString datapath                       = "";
        wxString tmppath                        = "";
        wxString thumbs_path                    = "";
        wxString language                       = "en";
        wxString output_filename_format         = "[mode]_[jobid]_[seed]_[width]x[height]";
        bool keep_model_in_memory               = true;
        bool save_all_image                     = true;
        bool auto_gen_hash                      = true;
        int n_threads                           = 2;
        imageTypes image_type                   = imageTypes::JPG;
        unsigned int image_quality              = 90;
        unsigned int png_compression_level      = 0;
        bool show_notifications                 = true;
        int notification_timeout                = 60;
        bool enable_civitai                     = true;
        wxString lastImageInfoPath              = "";
        wxString lastImg2ImgPath                = "";
        wxString lastUpscalerPath               = "";
        bool widgetVisible                      = false;
        int mainSashPose                        = 320;
        bool favorite_models_only               = false;
        bool remote_download_images_immediately = false;  // false: download job images on job item select | true: download job images immediately

        bool initServerList(wxEvtHandler* eventHandler) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (configBase->HasGroup("/Servers")) {
                wxString oldGroup = configBase->GetPath();
                configBase->SetPath("/Servers");
                long index;
                wxString serverKey;
                bool hasMore = configBase->GetFirstGroup(serverKey, index);

                while (hasMore) {
                    configBase->SetPath(serverKey);
                    wxString host   = configBase->Read("Host", wxEmptyString);
                    bool enabled    = configBase->ReadBool("Enabled", false);
                    wxString id     = configBase->Read("Id", wxEmptyString);
                    int port        = configBase->Read("Port", 8191);
                    int internal_id = configBase->Read("InternalId", -1);

                    wxString clientIdStr;
                    uint64_t client_id = 0;
                    if (this->configBase->Read("ClientId", &clientIdStr)) {
                        client_id = std::stoull(clientIdStr.ToStdString());
                    }

                    wxString name = configBase->Read("Name", wxEmptyString);

                    if (this->servers.contains(internal_id)) {
                        if (this->servers[internal_id] != nullptr &&
                            (this->servers[internal_id]->GetHost() != host.utf8_string() ||
                             this->servers[internal_id]->GetPort() != port ||
                             this->servers[internal_id]->IsEnabled() != enabled)) {
                            this->servers[internal_id]->Stop();
                        }
                    } else {
                        this->servers[internal_id] = new sd_gui_utils::sdServer(host.utf8_string(), port, eventHandler);
                    }

                    this->servers[internal_id]->SetEnabled(enabled);
                    this->servers[internal_id]->SetInternalId(internal_id);
                    this->servers[internal_id]->SetId(id.ToStdString());
                    this->servers[internal_id]->SetName(name.ToStdString());
                    this->servers[internal_id]->SetClientId(client_id);

                    configBase->SetPath("..");
                    hasMore = configBase->GetNextGroup(serverKey, index);
                }
                configBase->SetPath(oldGroup);
                return true;
            }
            return false;
        }

        inline void ServerEnable(int internal_id, bool enable, bool autostartstop = false) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                if (enable) {
                    this->servers[internal_id]->LoadAuthKeyFromSecretStore();
                }
                this->servers[internal_id]->SetEnabled(enable, autostartstop);
                return;
            }
        }

        inline sd_gui_utils::sdServer* GetTcpServer(int internal_id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return nullptr;
            }
            if (this->servers.contains(internal_id)) {
                return this->servers[internal_id];
            }

            return nullptr;
        }
        inline sd_gui_utils::sdServer* GetTcpServer(std::string server_id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return nullptr;
            }
            for (auto it = this->servers.begin(); it != this->servers.end(); ++it) {
                if ((*it).second->GetId() == server_id) {
                    return it->second;
                }
            }
            return nullptr;
        }
        inline void AddTcpServer(sd_gui_utils::sdServer* server) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (server->GetInternalId() == -1) {
                server->SetInternalId(this->servers.size());
            }
            while (this->servers.contains(server->GetInternalId())) {
                server->SetInternalId(server->GetInternalId() + 1);
            }
            this->servers[server->GetInternalId()] = server;
        }
        inline void RemoveTcpServer(int internal_id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->Stop();
                delete this->servers[internal_id];
                this->servers.erase(internal_id);
                return;
            }
        }
        inline bool ServerExist(const std::string& host, int port) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return false;
            }
            for (auto it = this->servers.begin(); it != this->servers.end(); ++it) {
                if (it->second->GetHost() == host && it->second->GetPort() == port) {
                    return true;
                }
            }
            return false;
        }

        inline void ServerChangePort(int internal_id, int port) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->SetPort(port);
            }
        }
        inline void ServerUpdateId(int internal_id, const std::string& id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->SetId(id);
            }
        }
        inline void ServerUpdateClientId(int internal_id, uint64_t newId) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->SetClientId(newId);
                auto key   = wxString::Format("/Servers/%s:%d/ClientId", this->servers[internal_id]->GetHost(), this->servers[internal_id]->GetPort());
                auto value = wxString::Format("%" PRIu64, this->servers[internal_id]->GetClientId());
                this->configBase->Write(key, value);
            }
        }
        inline void ServerChangeHost(int internal_id, const std::string& host) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->SetHost(host);
            }
        }
        inline void ServerChangeName(int internal_id, const std::string& name) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }
            if (this->servers.contains(internal_id)) {
                this->servers[internal_id]->SetName(name);
            }
        }

        inline void ClearTcpServers() {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->servers.empty()) {
                return;
            }

            for (auto it = this->servers.begin(); it != this->servers.end(); ++it) {
                delete it->second;
            }
            this->servers.clear();
        }

        inline std::vector<sd_gui_utils::sdServer*> ListRemoteServers() {
            std::lock_guard<std::mutex> lock(this->mutex);
            std::vector<sd_gui_utils::sdServer*> list;
            for (auto it = this->servers.begin(); it != this->servers.end(); ++it) {
                list.push_back(it->second);
            }
            return list;
        }

        inline const wxString getPathByDirType(const wxString& dirTypeName) {
            auto f = sd_gui_utils::dirtypes_wxstr.find(dirTypeName);
            if (f != sd_gui_utils::dirtypes_wxstr.end()) {
                return this->getPathByDirType(f->second);
            }
            return wxEmptyString;
        };
        inline const wxString getPathByDirType(sd_gui_utils::DirTypes dirType) {
            switch (dirType) {
                case sd_gui_utils::DirTypes::CHECKPOINT:
                    return this->model;
                case sd_gui_utils::DirTypes::VAE:
                    return this->vae;
                case sd_gui_utils::DirTypes::LORA:
                    return this->lora;
                case sd_gui_utils::DirTypes::EMBEDDING:
                    return this->embedding;
                case sd_gui_utils::DirTypes::TAESD:
                    return this->taesd;
                case sd_gui_utils::DirTypes::PRESETS:
                    return this->presets;
                case sd_gui_utils::DirTypes::PROMPT_TEMPLATES:
                    return this->prompt_templates;
                default:
                    return wxEmptyString;
            }
        };
        config(wxConfigBase* config)
            : configBase(config) {
            wxString datapath   = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "sd_ui_data" + wxFileName::GetPathSeparator();
            wxString imagespath = wxStandardPaths::Get().GetDocumentsDir() + wxFileName::GetPathSeparator() + "sd_ui_output" + wxFileName::GetPathSeparator();

            wxString model_path = datapath;
            model_path.append("checkpoints");

            wxString vae_path = datapath;
            vae_path.append("vae");

            wxString lora_path = datapath;
            lora_path.append("lora");

            wxString embedding_path = datapath;
            embedding_path.append("embedding");

            wxString taesd_path = datapath;
            taesd_path.append("taesd");

            wxString presets_path = datapath;
            presets_path.append("presets");

            wxString prompt_templates_path = datapath;
            prompt_templates_path.append("prompt_templates");

            wxString jobs_path = datapath;
            jobs_path.append("queue_jobs");

            wxString thumbs_path = datapath;
            thumbs_path.append("thumbs");

            wxString tmp_path = datapath;
            tmp_path.append("tmp");

            wxString controlnet_path = datapath;
            controlnet_path.append("controlnet");

            wxString esrgan_path = datapath;
            esrgan_path.append("esrgan");

            this->datapath = datapath;

            this->thumbs_path = thumbs_path;
            this->tmppath     = tmp_path;

            this->lora                               = config->Read("/paths/lora", lora_path);
            this->model                              = config->Read("/paths/model", model_path);
            this->vae                                = config->Read("/paths/vae", vae_path);
            this->embedding                          = config->Read("/paths/embedding", embedding_path);
            this->taesd                              = config->Read("/paths/taesd", taesd_path);
            this->esrgan                             = config->Read("/paths/esrgan", esrgan_path);
            this->controlnet                         = config->Read("/paths/controlnet", controlnet_path);
            this->presets                            = config->Read("/paths/presets", presets_path);
            this->prompt_templates                   = config->Read("/paths/prompt_templates", prompt_templates_path);
            this->jobs                               = config->Read("/paths/jobs", jobs_path);
            this->output                             = config->Read("/paths/output", imagespath);
            this->keep_model_in_memory               = config->Read("/keep_model_in_memory", this->keep_model_in_memory);
            this->save_all_image                     = config->Read("/save_all_image", this->save_all_image);
            this->n_threads                          = config->Read("/n_threads", cores());
            this->show_notifications                 = config->ReadBool("/show_notification", this->show_notifications);
            this->notification_timeout               = config->Read("/notification_timeout", this->notification_timeout);
            this->image_quality                      = config->Read("/image_quality", this->image_quality);
            this->enable_civitai                     = config->ReadBool("/enable_civitai", this->enable_civitai);
            this->language                           = config->Read("/language", wxUILocale::GetSystemLocaleId().GetLanguage());
            this->output_filename_format             = config->Read("/output_filename_format", this->output_filename_format);
            this->lastImageInfoPath                  = config->Read("/lastImageInfoPath", this->lastImageInfoPath);
            this->lastImg2ImgPath                    = config->Read("/lastImg2ImgPath", this->lastImg2ImgPath);
            this->lastUpscalerPath                   = config->Read("/lastUpscalerPath", this->lastUpscalerPath);
            this->auto_gen_hash                      = config->ReadBool("/auto_gen_hash", this->auto_gen_hash);
            this->widgetVisible                      = config->ReadBool("/widgetVisible", this->widgetVisible);
            this->mainSashPose                       = config->Read("/mainSashPose", this->mainSashPose);
            this->favorite_models_only               = config->ReadBool("/favorite_models_only", this->favorite_models_only);
            this->remote_download_images_immediately = config->ReadBool("/remote/download_images_immediately", this->remote_download_images_immediately);

            auto saved_image_type = config->Read("/image_type", "JPG");

            if (sd_gui_utils::image_types_str_reverse.contains(saved_image_type)) {
                this->image_type = sd_gui_utils::image_types_str_reverse.at(saved_image_type);
            }

            // check if directories exists
            if (wxFileName::DirExists(datapath) == false) {
                wxFileName(datapath).Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(model_path) == false) {
                wxFileName::Mkdir(model_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(thumbs_path) == false) {
                wxFileName::Mkdir(thumbs_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(vae_path) == false) {
                wxFileName::Mkdir(vae_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(lora_path) == false) {
                wxFileName::Mkdir(lora_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(embedding_path) == false) {
                wxFileName::Mkdir(embedding_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(controlnet_path) == false) {
                wxFileName::Mkdir(controlnet_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(esrgan_path) == false) {
                wxFileName::Mkdir(esrgan_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(taesd_path) == false) {
                wxFileName::Mkdir(taesd_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(presets_path) == false) {
                wxFileName::Mkdir(presets_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(prompt_templates_path) == false) {
                wxFileName::Mkdir(prompt_templates_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(jobs_path) == false) {
                wxFileName::Mkdir(jobs_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }

            if (wxFileName::DirExists(imagespath) == false) {
                wxFileName::Mkdir(imagespath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
            if (wxFileName::DirExists(tmppath) == false) {
                wxFileName::Mkdir(tmppath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
        }
        void FlushConfig() {
            if (this->configBase != nullptr) {
                this->configBase->Write("/paths/model", this->model);
                this->configBase->Write("/paths/lora", this->lora);
                this->configBase->Write("/paths/vae", this->vae);
                this->configBase->Write("/paths/embedding", this->embedding);
                this->configBase->Write("/paths/taesd", this->taesd);
                this->configBase->Write("/paths/esrgan", this->esrgan);
                this->configBase->Write("/paths/controlnet", this->controlnet);
                this->configBase->Write("/paths/presets", this->presets);
                this->configBase->Write("/paths/prompt_templates", this->prompt_templates);
                this->configBase->Write("/paths/jobs", this->jobs);
                this->configBase->Write("/paths/thumbs", this->thumbs_path);
                this->configBase->Write("/paths/tmp", this->tmppath);
                this->configBase->Write("/paths/datapath", this->datapath);
                this->configBase->Write("/paths/output", this->output);
                this->configBase->Write("/keep_model_in_memory", this->keep_model_in_memory);
                this->configBase->Write("/save_all_image", this->save_all_image);
                this->configBase->Write("/n_threads", this->n_threads);
                this->configBase->Write("/show_notification", this->show_notifications);
                this->configBase->Write("/notification_timeout", this->notification_timeout);
                this->configBase->Write("/image_quality", this->image_quality);
                this->configBase->Write("/enable_civitai", this->enable_civitai);
                this->configBase->Write("/auto_gen_hash", this->auto_gen_hash);
                this->configBase->Write("/widgetVisible", this->widgetVisible);
                this->configBase->Write("/mainSashPose", this->mainSashPose);
                this->configBase->Write("/favorite_models_only", this->favorite_models_only);
                this->configBase->Write("/remote/download_images_immediately", this->remote_download_images_immediately);
                this->configBase->Write("/image_type", sd_gui_utils::image_types_str.at(this->image_type));
                this->configBase->Write("/png_compression_level", this->png_compression_level);
                this->configBase->Write("/output_filename_format", this->output_filename_format);

                if (this->configBase->HasGroup("/Servers")) {
                    this->configBase->DeleteGroup("/Servers");
                }

                for (size_t i = 0; i < servers.size(); i++) {
                    if (servers[i] == nullptr) {
                        continue;
                    }
                    wxString pathPrefix = wxString::Format("/Servers/%s:%d", servers[i]->GetHost(), servers[i]->GetPort());
                    this->configBase->Write(pathPrefix + "/Host", wxString::FromUTF8Unchecked(servers[i]->GetHost()));
                    this->configBase->Write(pathPrefix + "/Port", servers[i]->GetPort());
                    this->configBase->Write(pathPrefix + "/Enabled", servers[i]->IsEnabled());
                    this->configBase->Write(pathPrefix + "/InternalId", servers[i]->GetInternalId());
                    this->configBase->Write(pathPrefix + "/Name", servers[i]->GetName());
                    this->configBase->Write(pathPrefix + "/Id", wxString(servers[i]->GetId().c_str()));
                    this->configBase->Write(pathPrefix + "/ClientId", wxString::Format("%" PRIu64, servers[i]->GetClientId()));
                }
                this->configBase->Flush();
            }
        }
        ~config() {
            this->FlushConfig();
            this->ClearTcpServers();
        }
    };

    inline std::string formatUnixTimestampToDate(long timestamp) {
        std::time_t time  = static_cast<std::time_t>(timestamp);
        std::tm* timeinfo = std::localtime(&time);

        std::stringstream ss;
        ss << (timeinfo->tm_year + 1900) << "-" << std::setw(2) << std::setfill('0')
           << (timeinfo->tm_mon + 1) << "-" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_mday << " " << std::setw(2) << std::setfill('0')
           << timeinfo->tm_hour << ":" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_min << ":" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_sec;  // Másodperc hozzáadása

        return ss.str();
    }

    struct generator_preset {
        double cfg;
        int seed;
        int clip_skip;
        int steps;
        int width, height;
        sample_method_t sampler;
        schedule_t scheduler;
        std::string name;
        std::string mode;  // modes_str
        int batch;
        std::string type;  // types_str
        bool read_only = false;
        std::string path;
    };

    struct LastItemNotification {
    private:
        QueueStatus status;
        uint64_t id = 0;
        std::mutex mutex;

    public:
        LastItemNotification() = default;
        bool changed(QueueStatus status, uint64_t id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            return this->status != status || this->id != id;
        }
        void update(QueueStatus status, uint64_t id) {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->status = status;
            this->id     = id;
        }
    };

    inline void to_json(nlohmann::json& j, const generator_preset& p) {
        j = nlohmann::json{
            {"cfg", p.cfg},
            {"seed", p.seed},
            {"clip_skip", p.clip_skip},
            {"steps", p.steps},
            {"width", p.width},
            {"height", p.height},
            {"name", p.name},
            {"mode", p.mode},
            {"type", p.type},
            {"read_only", p.read_only},
            {"sampler", (int)p.sampler},
            {"scheduler", (int)p.scheduler},
            {"batch", p.batch},
        };
    }

    inline void from_json(const nlohmann::json& j, generator_preset& p) {
        j.at("cfg").get_to(p.cfg);
        j.at("seed").get_to(p.seed);
        j.at("clip_skip").get_to(p.clip_skip);
        j.at("steps").get_to(p.steps);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
        j.at("name").get_to(p.name);
        j.at("mode").get_to(p.mode);
        if (j.contains("type")) {
            j.at("type").get_to(p.type);
        }
        if (j.contains("read_only")) {
            j.at("read_only").get_to(p.read_only);
        }
        j.at("batch").get_to(p.batch);
        p.sampler = j.at("sampler").get<sample_method_t>();
        if (j.contains("scheduler")) {
            p.scheduler = j.at("scheduler").get<schedule_t>();
        }
    }

    struct prompt_templates {
        std::string name;
        std::string prompt;
        std::string negative_prompt;
        std::string path;
        bool read_only = false;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(prompt_templates, name, prompt, negative_prompt, path, read_only)

    inline int GetCurrentUnixTimestamp() {
        const auto p1 = std::chrono::system_clock::now();
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
                .count());
    }
    inline bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    inline std::string normalizePath(const std::string& messyPath) {
        std::filesystem::path path(messyPath);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        std::string npath                   = canonicalPath.make_preferred().string();

        char toReplace   = '\\';
        char replacement = '/';

        std::replace(npath.begin(), npath.end(), toReplace, replacement);
        return npath;
    }
    // sd c++
    inline const char* sample_method_str[] = {
        "euler_a",
        "euler",
        "heun",
        "dpm2",
        "dpm++2s_a",
        "dpm++2m",
        "dpm++2mv2",
        "ipndm",
        "ipndm_v",
        "lcm",
        "ddim_trailing",
        "tcd",
    };
    inline const char* schedule_str[] = {
        "default",
        "discrete",
        "karras",
        "exponential",
        "ays",
        "gits",
    };

    inline const std::unordered_map<SDMode, wxString> SDModeGUINames = {
        {SDMode::TXT2IMG, _("TXT2IMG")},
        {SDMode::IMG2IMG, _("IMG2IMG")},
        {SDMode::CONVERT, _("CONVERT")},
        {SDMode::UPSCALE, _("UPSCALE")}};

    inline const std::unordered_map<sample_method_t, wxString> samplerUiName = {
        {sample_method_t::EULER, _("Euler")},
        {sample_method_t::EULER_A, _("Euler a")},
        {sample_method_t::DPM2, _("DPM2")},
        {sample_method_t::DPMPP2M, _("DPM++ 2M")},
        {sample_method_t::DPMPP2Mv2, _("DPM++ 2Mv2")},
        {sample_method_t::DPMPP2S_A, _("DPM++ 2S a")},
        {sample_method_t::HEUN, _("Heun")},
        {sample_method_t::LCM, _("LCM")},
        {sample_method_t::IPNDM, _("IPNDM")},
        {sample_method_t::IPNDM_V, _("IPNDM_V")},
        {sample_method_t::DDIM_TRAILING, _("DDIM Trailing")},
        {sample_method_t::TCD, _("TCD")},

    };

    inline const std::unordered_map<sample_method_t, std::string> samplerSdWebuiNames = {
        {sample_method_t::SAMPLE_METHOD_COUNT, "Automatic"},
        {sample_method_t::EULER, "Euler"},
        {sample_method_t::EULER_A, "Euler a"},
        {sample_method_t::DPM2, "DPM2"},
        {sample_method_t::DPMPP2M, "DPM++ 2M"},
        {sample_method_t::DPMPP2Mv2, "Automatic"},
        {sample_method_t::DPMPP2S_A, "DPM++ 2S a"},
        {sample_method_t::HEUN, "Heun"},
        {sample_method_t::LCM, "LCM"},
        {sample_method_t::IPNDM, "IPNDM"},
        {sample_method_t::IPNDM_V, "IPNDM_V"},
        {sample_method_t::DDIM_TRAILING, "DDIM Trailing"},
        {sample_method_t::TCD, "TCD"}
    };
    inline const std::unordered_map<schedule_t, std::string> schedulerSdWebuiNames = {
        {schedule_t::DEFAULT, "Automatic"},
        {schedule_t::SCHEDULE_COUNT, "Automatic"},
        {schedule_t::DISCRETE, "Automatic"},  // there is not discrete in sdwebui forge
        {schedule_t::KARRAS, "Karras"},
        {schedule_t::EXPONENTIAL, "Exponential"},
        {schedule_t::AYS, "Align Your Steps 11"},  // maybe this better
        {schedule_t::GITS, "Align Your Steps GITS"},
    };

    // f32, f16, q4_0, q4_1, q5_0, q5_1, q8_0
    inline std::map<int, std::string> sd_type_gui_names = {
        {SD_TYPE_F32, "F32"},
        {SD_TYPE_F16, "F16"},
        {SD_TYPE_Q4_0, "Q4_0"},
        {SD_TYPE_Q4_1, "Q4_1"},
        {SD_TYPE_Q5_0, "Q5_0"},
        {SD_TYPE_Q5_1, "Q5_1"},
        {SD_TYPE_Q8_0, "Q8_0"},
        {SD_TYPE_Q2_K, "Q2_K"},
        {SD_TYPE_Q3_K, "Q3_K"},
        {SD_TYPE_Q4_K, "Q4_K"}};

    inline std::map<schedule_t, wxString> sd_scheduler_gui_names = {
        {schedule_t::DEFAULT, "Default"},
        {schedule_t::DISCRETE, "Discrete"},
        {schedule_t::KARRAS, "Karras"},
        {schedule_t::EXPONENTIAL, "Exponential"},
        {schedule_t::AYS, "Ays"},
        {schedule_t::GITS, "Gits"}};

    // sd c++

    inline const unsigned int generateRandomInt(unsigned int min, unsigned int max) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<unsigned int> dis(min, max);
        return dis(gen);
    };

    inline std::string removeWhitespace(std::string str) {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos, str.end());
        return str;
    }
    inline std::string tolower(std::string data) {
        std::transform(data.begin(), data.end(), data.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return data;
    }
    inline std::unordered_map<wxString, wxString> parseExifPrompts(wxString text) {
        std::unordered_map<wxString, wxString> result;
        result["prompt"] = "";

        wxArrayString lines_in_reverse;
        wxStringInputStream inputStream(text);
        wxTextInputStream textStream(inputStream);
        wxString line;

        while (!inputStream.Eof()) {
            line = textStream.ReadLine();
            lines_in_reverse.Add(line);
        }

        std::reverse(lines_in_reverse.begin(), lines_in_reverse.end());

        if (lines_in_reverse.IsEmpty()) {
            return result;
        }

        wxArrayString seglist;
        wxString firstline = lines_in_reverse[0];
        wxStringTokenizer tokenizer(firstline, ",");

        while (tokenizer.HasMoreTokens()) {
            wxString segment = tokenizer.GetNextToken();
            size_t pos       = segment.find(':');
            if (pos != wxString::npos) {
                wxString key = segment.SubString(0, pos - 1).Trim().MakeLower();
                key.Trim(false);
                wxString val = segment.SubString(pos + 1, segment.size() - 1).Trim();
                val.Trim(false);
                result[key] = val;
            }
        }

        lines_in_reverse.RemoveAt(0);

        bool negativePromptFound = false;
        int counter              = 0;
        wxArrayString negativePromptLines;
        wxString negativePromptMarker = "Negative prompt: ";

        for (size_t i = 0; i < lines_in_reverse.size(); ++i) {
            wxString line = lines_in_reverse[i];
            if (line.StartsWith(negativePromptMarker)) {
                negativePromptLines.Add(line.Mid(negativePromptMarker.size()));
                negativePromptFound = true;
                counter++;
                break;
            } else {
                negativePromptLines.Add(line);
                counter++;
            }
        }

        for (int i = 0; i < counter; ++i) {
            lines_in_reverse.RemoveAt(0);
        }

        wxString prompt;

        for (size_t i = 0; i < negativePromptLines.size(); ++i) {
            prompt += negativePromptLines[negativePromptLines.size() - i - 1];
        }

        if (negativePromptFound) {
            result["negative_prompt"] = prompt;
            for (size_t i = 0; i < lines_in_reverse.size(); ++i) {
                result["prompt"] += lines_in_reverse[i];
            }
        } else {
            result["prompt"] = prompt;
        }

        return result;
    }

    inline wxString Exiv2ToWxString(const std::string& exifString) {
        const char* prefix_ascii1  = "charset=\"ASCII\" ";
        const char* prefix_ascii2  = "charset=Ascii ";
        const char* prefix_Unicode = "charset=Unicode ";
        wxString info;

        const size_t input_size = exifString.size();

        if (wxString::FromUTF8(exifString).StartsWith(prefix_ascii1)) {
            const size_t prefix1_size = strlen(prefix_ascii1);
            if (input_size > prefix1_size) {
                info = wxString(exifString.c_str() + prefix1_size, wxConvISO8859_1);
            }
        } else if (wxString::FromUTF8(exifString).StartsWith(prefix_ascii2)) {
            const size_t prefix2_size = strlen(prefix_ascii2);
            if (input_size > prefix2_size) {
                info = wxString(exifString.c_str() + prefix2_size, wxConvISO8859_1);
            }
        } else if (wxString::FromUTF8(exifString).StartsWith(prefix_Unicode)) {
            const size_t prefixunicode_size = strlen(prefix_Unicode);
            if (input_size > prefixunicode_size) {
                info = wxString::FromUTF8(exifString.c_str() + prefixunicode_size, input_size - prefixunicode_size);
            }
        } else if (input_size > 0) {
            info = wxString::FromUTF8(exifString.c_str(), input_size);
        }

        return info;
    }

    inline static wxString AppendSuffixToFileName(const wxString& filePath, const wxString& suffix) {
        if (!wxFileExists(filePath)) {
            std::cerr << "File does not exist: " << filePath << std::endl;
            return "";
        }

        wxFileName fileName(filePath);

        wxString baseName = fileName.GetName();
        fileName.SetName(baseName + "_" + suffix);

        return fileName.GetFullPath();
    }
    inline static wxString formatFileName(const QueueItem& item, const wxString& format = "[mode]_[jobid]_[seed]_[width]x[height]_[batch]", wxString server_name = "") {
        wxDateTime localTime = wxDateTime::Now();

        if (item.finished_at > 0) {
            localTime.Set(static_cast<time_t>(item.finished_at));
        } else if (item.updated_at > 0) {
            localTime.Set(static_cast<time_t>(item.updated_at));
        } else if (item.created_at > 0) {
            localTime.Set(static_cast<time_t>(item.created_at));
        }

        auto day     = localTime.Format(wxT("%d"));
        auto month   = localTime.Format(wxT("%m"));
        auto year    = localTime.Format(wxT("%Y"));
        auto hours   = localTime.Format(wxT("%H"));
        auto minutes = localTime.Format(wxT("%M"));
        auto seconds = localTime.Format(wxT("%S"));

        std::unordered_map<std::string, std::string> tags = {
            {"[year]", year.ToStdString()},
            {"[month]", month.ToStdString()},
            {"[day]", day.ToStdString()},
            {"[hours]", hours.ToStdString()},
            {"[minutes]", minutes.ToStdString()},
            {"[seconds]", seconds.ToStdString()},
            {"[finished_at]", std::to_string(item.finished_at)},
            {"[created_at]", std::to_string(item.created_at)},
            {"[updated_at]", std::to_string(item.updated_at)},
            {"[jobid]", std::to_string(item.id)},
            {"[seed]", std::to_string(item.params.seed)},
            {"[batch]", std::to_string(item.params.batch_count)},
            {"[width]", std::to_string(item.params.width)},
            {"[height]", std::to_string(item.params.height)},
            {"[mode]", modes_str[item.mode]},
            {"[model]", item.model},
            {"[model_sha256]", item.sha256},
            {"[steps]", std::to_string(item.step)},
            {"[steps_total]", std::to_string(item.steps)},
            {"[cfg_scale]", std::to_string(item.params.cfg_scale)},
            {"[denoising_strength]", std::to_string(item.params.strength)},
            {"[server_id]", item.server},
            {"[server_name]", server_name.ToStdString()}};

        wxString result = format;

        for (const auto& [tag, value] : tags) {
            size_t pos = 0;
            while ((pos = result.find(tag, pos)) != std::string::npos) {
                result.replace(pos, tag.length(), value);
                pos += value.length();
            }
        }

        return result;
    }

    /**
     * Creates a unique file path by combining the given filename, extension, and folder path.
     * If a file with the same name already exists, appends a counter to generate a unique filename.
     * Optionally appends a suffix to the filename.
     *
     * @param filename The base name of the file.
     * @param extension The file extension.
     * @param folderPath The path to the folder where the file will be located.
     * @param suffix An optional suffix to append to the filename.
     * @return The full path of the unique file as a wxString.
     *
     * If the specified directory does not exist, it is created recursively.
     * Handles different path separators based on the operating system.
     * Logs an error message if the directory cannot be created.
     */

    inline static wxString CreateFilePath(const wxString& filename, const wxString& extension, const wxString& folderPath, const wxString& suffix = "", bool rename_if_exists = false) {
        // Normalize path separator for the current OS
        wxString normalizedFilename = folderPath + wxFileName::GetPathSeparator() + filename + extension;
        normalizedFilename.Replace("\\", wxFileName::GetPathSeparator());
        normalizedFilename.Replace("/", wxFileName::GetPathSeparator());

        // Combine folder path and filename
        // wxFileName fullPath(folderPath, normalizedFilename, extension);
        wxFileName fullPath(normalizedFilename);

        // Ensure the parent directory exists
        wxString parentDir = fullPath.GetPath();

        // Check if the file exists and generate a unique filename
        wxString baseName = fullPath.GetName();  // Extract base name (e.g., "mi")
        wxString ext      = fullPath.GetExt();   // Extract extension (e.g., "txt")
        wxString path     = fullPath.GetPath();  // Extract path (e.g., "/home/valaki/kiement/vala")

        if (!wxDirExists(path)) {
            // create directories recursively
            if (!wxFileName::Mkdir(path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                std::cerr << "Failed to create directory: " << path << std::endl;
            }
        }
        if (rename_if_exists) {
            int counter = 0;
            while (wxFileExists(fullPath.GetFullPath())) {
                // Generate a new unique name: baseName_counter_suffix.extension
                wxString newName = wxString::Format("%s_%d", baseName, counter);
                fullPath.Assign(path, newName, ext);
                ++counter;
            }
        }

        if (!suffix.IsEmpty()) {
            return AppendSuffixToFileName(fullPath.GetFullPath(), suffix);
        }
        // Return the unique full file path as a string
        return fullPath.GetFullPath();
    }

    inline schedule_t FindSchedulerFromString(const wxString& str) {
        for (const auto s : sd_gui_utils::schedulerSdWebuiNames) {
            if (s.second == str) {
                return s.first;
            }
        }
        for (const auto s : sd_gui_utils::schedulerSdWebuiNames) {
            if (s.second == str) {
                return s.first;
            }
        }
        int counter = 0;
        for (const auto s : sd_gui_utils::schedule_str) {
            if (str.Contains(s)) {
                return schedule_t(counter);
            }
            counter++;
        }
        return schedule_t::DEFAULT;
    }

    inline sample_method_t FindSamplerFromString(const wxString& str) {
        for (const auto s : sd_gui_utils::samplerSdWebuiNames) {
            if (s.second == str) {
                return s.first;
            }
        }

        int counter = 0;
        for (const auto s : sd_gui_utils::sample_method_str) {
            if (str.Contains(s)) {
                return sample_method_t(counter);
            }
            counter++;
        }
        return sample_method_t::EULER;  // euler is the default
    }

    static wxString formatSecs(double sec) {
        wxTimeSpan timeSpan    = wxTimeSpan::Seconds(static_cast<wxLongLong>(sec));
        wxString formattedTime = timeSpan.Format();  // Lokalizált formátum
        return formattedTime;
    }

    class IntClientData : public wxClientData {
    public:
        IntClientData(int id)
            : m_id(id) { ; }
        int getId() const { return m_id; }

    private:
        int m_id;
    };

    class StringClientData : public wxClientData {
    public:
        StringClientData(std::string str)
            : m_str(str) { ; }
        std::string getStr() const { return m_str; }

    private:
        std::string m_str;
    };

    struct wxPointDouble {
        wxPointDouble()                                = default;
        wxPointDouble(const wxPointDouble&)            = default;
        wxPointDouble(wxPointDouble&&)                 = default;
        wxPointDouble& operator=(const wxPointDouble&) = default;
        wxPointDouble& operator=(wxPointDouble&&)      = default;
        wxPointDouble& operator=(wxPoint&&) {
            x = static_cast<double>(x);
            y = static_cast<double>(y);
            return *this;
        }
        wxPointDouble& operator=(wxPoint&) {
            x = static_cast<double>(x);
            y = static_cast<double>(y);
            return *this;
        }
        wxPointDouble(wxPoint pos)
            : x(pos.x), y(pos.y) {}
        wxPointDouble(double x, double y)
            : x(x), y(y) {}
        double x = 0;
        double y = 0;
    };
    struct wxPosition {
        wxPosition(sd_gui_utils::wxPointDouble pos, wxPoint offset, sd_gui_utils::wxPointDouble originalPos)
            : pos(pos), offset(offset), originalPos(originalPos) {}
        wxPosition() = default;
        sd_gui_utils::wxPointDouble pos;
        wxPoint offset;
        wxPointDouble originalPos;
    };

    /*
     * Used at m_notebook1302OnNotebookPageChanged
     * FYI: keep the order as the gui builder
     */
    enum class GuiMainPanels : int {
        PANEL_QUEUE,
        PANEL_TEXT2IMG,
        PANEL_IMG2IMG,
        PANEL_UPSCALER,
        PANEL_IMAGEINFO,
        PANEL_MODELS
    };

    /// \brief Convert GuiMainPanels enum to size_t
    ///
    /// This implicit conversion is useful when using the enum as an index to a
    /// container, such as a vector or array.
    ///
    /// \param[in] panel The GuiMainPanels enum value to convert
    /// \return The size_t value corresponding to the enum
    constexpr size_t operator+(GuiMainPanels panel) {
        return static_cast<size_t>(panel);
    }
    constexpr bool operator==(int value, GuiMainPanels panel) {
        return value == static_cast<int>(panel);
    }

    constexpr bool operator!=(int value, GuiMainPanels panel) {
        return value != static_cast<int>(panel);
    }

    enum class GuiPromptStyles : int {
        STYLE_LORA           = 10,
        STYLE_EMBEDDING      = 11,
        STYLE_OTHERS         = 12,
        STYLE_LORA_NOT_FOUND = 16

    };

    constexpr int operator+(GuiPromptStyles style) {
        return static_cast<int>(style);
    }

    /**
     * \brief Checks if a given version string is compatible with the current version.
     *
     * This function compares the provided version string, which should follow a
     * dot-separated format, with the current software version defined by SD_GUI_VERSION.
     * It removes the dots and converts both versions to integers for comparison.
     *
     * \param[in] version The version string to check for compatibility.
     * \return True if the provided version is the same as the current version;
     * false otherwise or if there is an error in the conversion process.
     */
    inline static bool CheckVersionCompatibility(const wxString& version) {
        if (version.Contains(".") == false) {
            return false;
        }
        wxString vversion(version);

        vversion.Replace(".", "");

        wxString currentVersion(SD_GUI_VERSION);
        currentVersion.Replace(".", "");
        int cver;
        if (!currentVersion.ToInt(&cver)) {
            return false;
        }
        int vver;
        if (!vversion.ToInt(&vver)) {
            return false;
        }
        return cver >= vver;  // TODO: check if this is correct
    }
}  // namespace sd_gui_utils
#endif
