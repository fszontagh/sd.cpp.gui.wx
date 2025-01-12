#include "ModelInfo.h"

ModelInfo::Manager::Manager(const wxString& meta_base_path) {
    this->MetaStorePath = meta_base_path + wxFileName::GetPathSeparators() + "modelinfos";

    if (!wxFileName::Exists(this->MetaStorePath)) {
        wxFileName::Mkdir(this->MetaStorePath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
}

ModelInfo::Manager::~Manager() {
    for (auto& pair : ModelInfos) {
        delete pair.second;
    }
    ModelInfos.clear();
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::addModel(wxString mpath, sd_gui_utils::DirTypes type, wxString basepath) {
    std::unique_lock<std::mutex> lock(mutex);
    wxString _meta = this->GetMetaPath(mpath);

    if (_meta.empty()) {
        return nullptr;
    }

    wxString name = mpath;
    name.Replace(basepath + wxFileName::GetPathSeparator(), "");

    if (name.SubString(name.Length() - 1, name.Length()) == wxFileName::GetPathSeparator()) {
        name = name.SubString(0, name.Length() - 1);
    }

    wxFileName meta_path = wxFileName(_meta);

    auto model_path = wxFileName(mpath);

    auto folderGroupName = this->GetFolderName(mpath, type, basepath);

    if (meta_path.FileExists()) {
        wxFile input;
        try {
            if (!input.Open(meta_path.GetAbsolutePath()) || !input.IsOpened()) {
                wxLogError("Could not open meta file for reading: %s", meta_path.GetAbsolutePath());
                input.Close();
                return nullptr;
            }
            wxString data_str;
            input.ReadAll(&data_str);

            nlohmann::json data             = nlohmann::json::parse(data_str.utf8_string());
            sd_gui_utils::ModelFileInfo* _z = new sd_gui_utils::ModelFileInfo(data.get<sd_gui_utils::ModelFileInfo>());

            if (meta_path.GetModificationTime() < model_path.GetModificationTime()) {
                this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = this->GenerateMeta(model_path, type, name);
            } else {
                this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = std::move(_z);
            }

            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->path = mpath;
            // always reqwrite, if the model is moved into deeper path, then we can redetect
            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->folderGroupName = folderGroupName.utf8_string();

            input.Close();
            this->ModelCount[type]++;
            return this->ModelInfos[model_path.GetAbsolutePath().utf8_string()];

        } catch (const std::exception& e) {
            wxLogError("Exception: %s file: %s", e.what(), model_path.GetAbsolutePath());
            input.Close();
            return nullptr;
        }
    } else {
        // no meta found, create a new one
        sd_gui_utils::ModelFileInfo* meta                            = this->GenerateMeta(model_path, type, name);
        meta->folderGroupName                                        = folderGroupName.utf8_string();
        this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = std::move(meta);
        this->WriteIntoMeta(meta);
        this->ModelCount[type]++;
        return this->ModelInfos[model_path.GetAbsolutePath().utf8_string()];
    }
    return nullptr;
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::addRemoteModel(const sd_gui_utils::networks::RemoteModelInfo& info, sd_gui_utils::sdServer* server) {
    std::unique_lock<std::mutex> lock(mutex);

    auto meta_file = this->GetMetaPath(info.path, true);
    if (wxFileExists(meta_file)) {
        wxFile input(meta_file, wxFile::read);
        wxString meta_content;
        if (input.ReadAll(&meta_content)) {
            try {
                nlohmann::json data             = nlohmann::json::parse(meta_content.utf8_string());
                sd_gui_utils::ModelFileInfo* _z = new sd_gui_utils::ModelFileInfo(data.get<sd_gui_utils::ModelFileInfo>());
                _z->folderGroupName             = this->GetFolderName(wxString::FromUTF8Unchecked(info.path), info.model_type, info.root_path, server);
                _z->meta_file                   = meta_file.utf8_string();
                _z->server_id                   = server->GetId();
                *_z += info; // merge with the new info
                if (_z->sha256.empty()) {
                    auto localVer = this->findInfoByName(info.name);
                    if (localVer != nullptr && localVer->sha256.empty() == false) {
                        _z->sha256 = localVer->sha256;
                        this->WriteIntoMeta(_z);
                    }
                }
                this->ModelInfos[info.path] = _z;
                this->ModelCount[info.model_type]++;
                return _z;
            } catch (const std::exception& e) {
                wxLogError("Exception: %s file: %s", e.what(), meta_file);
            }
        }
    }
    sd_gui_utils::ModelFileInfo* meta = new sd_gui_utils::ModelFileInfo(info);
    meta->meta_file                   = meta_file.utf8_string();
    meta->folderGroupName             = this->GetFolderName(wxString::FromUTF8Unchecked(info.path), info.model_type, info.root_path, server);

    auto localVer = this->findInfoByName(info.name);
    if (localVer != nullptr && localVer->sha256.empty() == false) {
        meta->sha256 = localVer->sha256;
    }
    meta->server_id = server->GetId();

    this->WriteIntoMeta(meta);

    this->ModelCount[info.model_type]++;
    this->ModelInfos[info.path] = meta;
    return meta;
}

bool ModelInfo::Manager::exists(std::string model_path) {
    std::unique_lock<std::mutex> lock(mutex);
    if (this->ModelInfos.find(model_path) != this->ModelInfos.end()) {
        return true;
    }
    return false;
}

void ModelInfo::Manager::setHash(std::string model_path, std::string hash) {
    std::unique_lock<std::mutex> lock(mutex);
    if (this->ModelInfos.contains(model_path)) {
        this->ModelInfos[model_path]->sha256 = hash;
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::getInfo(std::string path) {
    if (this->ModelInfos.find(path) != this->ModelInfos.end()) {
        return *this->ModelInfos[path];
    }
    return sd_gui_utils::ModelFileInfo();
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::getIntoPtr(std::string path) {
    if (this->exists(path)) {
        return this->ModelInfos[path];
    }
    return nullptr;
}
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::getIntoPtrByHash(std::string hash) {
    if (hash.length() == 10) {
        for (auto model : this->ModelInfos) {
            if (model.second->sha256.substr(0, 10) == hash) {
                return model.second;
            }
        }
        return nullptr;
    }
    for (auto model : this->ModelInfos) {
        if (model.second->sha256 == hash) {
            return model.second;
        }
    }
    return nullptr;
}
// @brief Get a modelinfo by it's name
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::getInfoByName(std::string model_name) {
    for (std::map<std::string, sd_gui_utils::ModelFileInfo*>::iterator itr = this->ModelInfos.begin(); itr != this->ModelInfos.end(); itr++) {
        if ((itr)->second->name == model_name) {
            return (itr)->second;
        }
    }

    return nullptr;
}
// @brief Find a similar named modelinfo
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::findInfoByName(std::string model_name) {
    for (auto model : this->ModelInfos) {
        if (model.second->name.find(model_name) != std::string::npos) {
            return model.second;
        }
    }

    return nullptr;
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::searchByName(const std::string& keyword, const sd_gui_utils::DirTypes& type) {
    for (auto model : this->ModelInfos) {
        if (model.second->model_type != type) {
            continue;
        }
        if (model.first.ends_with(keyword) ||
            model.second->name.ends_with(keyword) ||
            model.first == keyword) {
            return model.second;
        }
    }
    return nullptr;
}
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::searchByName(const std::vector<std::string>& keywords, const sd_gui_utils::DirTypes& type) {
    for (auto model : this->ModelInfos) {
        if (model.second->model_type != type) {
            continue;
        }
        for (auto keyword : keywords) {
            auto f = this->searchByName(keyword, type);
            if (f != nullptr) {
                return f;
            }
        }
    }
    return nullptr;
}
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::findModelByImageParams(const std::unordered_map<wxString, wxString>& params) {
    bool modelFound = false;
    for (const auto& item : params) {
        // get by hash
        if ((item.first == "modelhash" || item.first == "model hash") && !modelFound) {
            auto check = this->getIntoPtrByHash(item.second.utf8_string());
            if (check != nullptr) {
                return check;
            }
        }

        if (item.first == "model" && !modelFound) {
            // get by name
            auto check = this->getInfoByName(item.second.utf8_string());
            if (check != nullptr) {
                return check;
            }

            // search by name
            auto check2 = this->findInfoByName(item.second.utf8_string());
            if (check2 != nullptr) {
                return check2;
            }
        }
    }
    return nullptr;
}
void ModelInfo::Manager::updateCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        return;
    }
    this->ParseCivitAiInfo(modelinfo);
    this->WriteIntoMeta(modelinfo);
}

void ModelInfo::Manager::UpdateInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    this->WriteIntoMeta(modelinfo);
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::GenerateMeta(const wxFileName& model_path, sd_gui_utils::DirTypes type, const wxString& name, sd_gui_utils::ModelFileInfo copyMeta) {
    auto path = model_path.GetFullPath().ToStdString();

    sd_gui_utils::ModelFileInfo* mi = new sd_gui_utils::ModelFileInfo();

    mi->size               = model_path.GetSize().GetValue();
    mi->size_f             = model_path.GetHumanReadableSize().ToStdString();
    mi->hash_fullsize      = 0;
    mi->hash_progress_size = 0;
    mi->model_type         = type;

    mi->meta_file = this->GetMetaPath(model_path.GetAbsolutePath());
    mi->name      = model_path.GetFullName();
    mi->path      = path;
    return mi;
}

void ModelInfo::Manager::WriteIntoMeta(const std::string& model_path) {
    if (this->exists(model_path)) {
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

void ModelInfo::Manager::WriteIntoMeta(const sd_gui_utils::ModelFileInfo& modelinfo) {
    nlohmann::json j(modelinfo);
    std::ofstream file(modelinfo.meta_file);
    file << j;
    file.close();
}

void ModelInfo::Manager::WriteIntoMeta(sd_gui_utils::ModelFileInfo* modelinfo) {
    this->WriteIntoMeta(*modelinfo);
}

void ModelInfo::Manager::ParseCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        return;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(modelinfo->civitaiPlainJson);
        if (data.contains("error")) {
            modelinfo->state = sd_gui_utils::CivitAiState::NOT_FOUND;
            return;
        }
        auto civitaiinfo       = data.template get<CivitAi::ModelInfo>();
        modelinfo->CivitAiInfo = civitaiinfo;
        modelinfo->state       = sd_gui_utils::CivitAiState::OK;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "message: " << e.what() << '\n'
                  << "exception id: " << e.id << '\n'
                  << "byte position of error: " << e.byte << std::endl;
        modelinfo->state = sd_gui_utils::CivitAiState::ERR;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << e.what() << '\n';
        modelinfo->state = sd_gui_utils::CivitAiState::ERR;
    }
}

wxString ModelInfo::Manager::GetMetaPath(const wxString& model_path, bool remote) {
    if (model_path.IsEmpty()) {
        return wxEmptyString;
    }

    wxString path_name = remote ? model_path.SubString(65, model_path.Length()) : model_path;
    wxFileName path(path_name);
    path.SetExt("json");
    wxFileName meta_path(this->MetaStorePath, path.GetFullName());

    if (remote) {
        meta_path.SetName(meta_path.GetName() + "_remote");
    }

    return meta_path.GetAbsolutePath();
}

wxString ModelInfo::Manager::GetFolderName(const wxString& model_path, const sd_gui_utils::DirTypes& type, wxString root_path, const sd_gui_utils::sdServer* server) {
    wxString path = model_path;
    if (server != nullptr) {
        path = model_path.SubString(65, model_path.Length());
    }
    auto folderGroupName = wxFileName(path).GetPath();

    folderGroupName.Replace(root_path, "", false);
    if (folderGroupName.StartsWith(wxFileName::GetPathSeparator())) {
        folderGroupName = folderGroupName.SubString(1, folderGroupName.Length());
    }

    if (!folderGroupName.empty() && !this->folderGroupExists(folderGroupName)) {
        wxFileName group_full_path(wxFileName(model_path).GetPath());
        if (!group_full_path.DirExists()) {
            group_full_path.AssignDir(wxFileName(model_path).GetPath());
        }
        // Add to folderGroups
        this->folderGroups[group_full_path.GetFullPath()] = group_full_path;
    }

    folderGroupName.Prepend(sd_gui_utils::dirtypes_str.at(type) + wxFileName::GetPathSeparator());
    if (server != nullptr) {
        folderGroupName.Prepend(server->GetName() + wxFileName::GetPathSeparator());
    } else {
        folderGroupName.Prepend(wxString("local") + wxFileName::GetPathSeparator());
    }

    return folderGroupName;
}
