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
        sd_gui_utils::ModelFileInfo* NameStartsWith(const wxString& keyword, const sd_gui_utils::DirTypes& type, std::string server = "");
        sd_gui_utils::ModelFileInfo* searchByName(const std::vector<std::string>& keywords, const sd_gui_utils::DirTypes& type);
        sd_gui_utils::ModelFileInfo* findModelByImageParams(const std::unordered_map<wxString, wxString>& params);
        sd_gui_utils::ModelFileInfo* findLocalModelOnRemote(sd_gui_utils::ModelFileInfo* model, sd_gui_utils::sdServer* server);
        sd_gui_utils::ModelFileInfo* findRemoteModelOnLocal(sd_gui_utils::ModelFileInfo* model, sd_gui_utils::sdServer* server);
        [[nodiscard]] const std::map<std::string, std::string> ListModelNamesByType(sd_gui_utils::DirTypes type, const wxString& filter = wxEmptyString);
        void resetModels(sd_gui_utils::DirTypes type);
        void UnloadModelsByServer(sd_gui_utils::sdServer* server);
        void deleteModel(std::string model_path);
        bool folderGroupExists(const wxString& group_name);
        const std::unordered_map<wxString, wxFileName> getFolderGroups() const;
        void updateCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo);
        void UpdateInfo(sd_gui_utils::ModelFileInfo* modelinfo);
        unsigned int GetCount(sd_gui_utils::DirTypes type);
    };
};
#endif