#ifndef __SD_GUI_MODEL_INFO__
#define __SD_GUI_MODEL_INFO__
#include <filesystem>
#include <fstream>
#include <vector>
#include "utils.hpp"

#include "../helpers/civitai.hpp"

namespace ModelInfo {
    class Manager {
    private:
        std::mutex mutex;
        std::map<std::string, sd_gui_utils::ModelFileInfo*> ModelInfos;
        sd_gui_utils::ModelFileInfo* GenerateMeta(const wxFileName& model_path, sd_gui_utils::DirTypes type, const wxString& name, sd_gui_utils::ModelFileInfo copyMeta = sd_gui_utils::ModelFileInfo());
        void WriteIntoMeta(const std::string& model_path);
        void WriteIntoMeta(const sd_gui_utils::ModelFileInfo& modelinfo);
        void WriteIntoMeta(sd_gui_utils::ModelFileInfo* modelinfo);
        void ParseCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo);
        wxString GetMetaPath(const wxString& model_path, bool remote = false);
        wxString GetFolderName(const wxString& model_path, const sd_gui_utils::DirTypes& type, wxString root_path, const sd_gui_utils::sdServer* server = nullptr);
        wxString MetaStorePath;
        std::unordered_map<sd_gui_utils::DirTypes, unsigned int> ModelCount;
        // fullpath -> wxFileName, the group name is the folder name
        std::unordered_map<wxString, wxFileName> folderGroups;

    public:
        Manager(const wxString& meta_base_path);
        ~Manager();
        sd_gui_utils::ModelFileInfo* addModel(wxString mpath, sd_gui_utils::DirTypes type, wxString basepath);
        sd_gui_utils::ModelFileInfo* addRemoteModel(const sd_gui_utils::networks::RemoteModelInfo& info, sd_gui_utils::sdServer* server);
        bool exists(std::string model_path);
        void setHash(std::string model_path, std::string hash);
        sd_gui_utils::ModelFileInfo getInfo(std::string path);
        sd_gui_utils::ModelFileInfo* getIntoPtr(std::string path);
        sd_gui_utils::ModelFileInfo* getIntoPtrByHash(std::string, std::string remote_server_id = "");
        sd_gui_utils::ModelFileInfo* getInfoByName(std::string model_name, const sd_gui_utils::DirTypes& type = sd_gui_utils::DirTypes::CHECKPOINT);
        sd_gui_utils::ModelFileInfo* findInfoByName(std::string model_name, const sd_gui_utils::DirTypes& type = sd_gui_utils::DirTypes::CHECKPOINT);
        sd_gui_utils::ModelFileInfo* searchByName(const std::string& keyword, const sd_gui_utils::DirTypes& type);
        inline sd_gui_utils::ModelFileInfo* NameStartsWith(const wxString& keyword, const sd_gui_utils::DirTypes& type) {
            std::lock_guard<std::mutex> lock(this->mutex);
            for (const auto& m : this->ModelInfos) {
                if (m.second->model_type == type && m.second->name.starts_with(keyword)) {
                    return m.second;
                }
            }
            return nullptr;
        }
        sd_gui_utils::ModelFileInfo* searchByName(const std::vector<std::string>& keywords, const sd_gui_utils::DirTypes& type);
        sd_gui_utils::ModelFileInfo* findModelByImageParams(const std::unordered_map<wxString, wxString>& params);
        sd_gui_utils::ModelFileInfo* findLocalModelOnRemote(sd_gui_utils::ModelFileInfo* model, sd_gui_utils::sdServer* server);
        sd_gui_utils::ModelFileInfo* findRemoteModelOnLocal(sd_gui_utils::ModelFileInfo* model, sd_gui_utils::sdServer* server);
        [[nodiscard]] const inline std::map<std::string, std::string> ListModelNamesByType(sd_gui_utils::DirTypes type, const wxString& filter = wxEmptyString) {
            std::map<std::string, std::string> list;
            for (const auto& it : this->ModelInfos) {
                wxString modelName(it.second->name);
                wxString modelNameL = modelName.Lower();
                if (it.second->model_type == type) {
                    if (filter.empty() == false && modelNameL.StartsWith(filter.Lower()) == false) {
                        continue;
                    }
                    // cut the name on the last .

                    if (modelName.Contains('.')) {
                        modelName = modelName.BeforeLast('.');
                    }
                    list[it.second->name] = modelName.ToStdString();
                }
            }
            return list;
        }
        inline void resetModels(sd_gui_utils::DirTypes type) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->ModelInfos.empty()) {
                return;
            }
            for (auto it = this->ModelInfos.begin(); it != this->ModelInfos.end();) {
                if (it->second->model_type == type) {
                    if (it->second) {
                        delete it->second;
                    }
                    it->second = nullptr;
                    it         = this->ModelInfos.erase(it);
                } else {
                    ++it;
                }
            }
            if (this->ModelCount.contains(type)) {
                this->ModelCount[type] = 0;
            }
        }
        inline void UnloadModelsByServer(sd_gui_utils::sdServer* server) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->ModelInfos.empty()) {
                return;
            }
            for (auto it = this->ModelInfos.begin(); it != this->ModelInfos.end();) {
                if (server->GetId().empty() == false && it->second->server_id == server->GetId()) {
                    this->ModelCount[it->second->model_type]--;
                    delete it->second;
                    it = this->ModelInfos.erase(it);
                } else {
                    it++;
                }
            }
        }
        inline void deleteModel(std::string model_path) {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->ModelInfos.contains(model_path)) {
                auto model = this->ModelInfos[model_path];
                if (model->CivitAiInfo.images.empty() == false) {
                    for (auto image : model->CivitAiInfo.images) {
                        if (wxFileName::Exists(wxString::FromUTF8Unchecked(image.local_path))) {
                            wxRemoveFile(wxString::FromUTF8Unchecked(image.local_path));
                        }
                    }
                }
                // delete the meta file
                if (wxFileName::Exists(wxString::FromUTF8Unchecked(model->meta_file))) {
                    wxRemoveFile(wxString::FromUTF8Unchecked(model->meta_file));
                }
                if (wxFileName::Exists(wxString::FromUTF8Unchecked(model_path))) {
                    if (wxRemoveFile(wxString::FromUTF8Unchecked(model_path))) {
                        delete model;
                        model = nullptr;
                        this->ModelInfos.erase(model_path);
                    }
                }
            }
        }
        inline bool folderGroupExists(const wxString& group_name) {
            for (const auto& group : this->folderGroups) {
                if (group.second.GetName() == group_name) {
                    return true;
                }
            }
            return false;
        }
        inline const std::unordered_map<wxString, wxFileName> getFolderGroups() const { return this->folderGroups; }

        void updateCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo);
        void UpdateInfo(sd_gui_utils::ModelFileInfo* modelinfo);
        inline unsigned int GetCount(sd_gui_utils::DirTypes type) {
            if (this->ModelCount.contains(type)) {
                return this->ModelCount[type];
            }
            return 0;
        }
    };
};
#endif