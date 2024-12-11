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
        wxString GetMetaPath(const wxString& model_path);
        wxString MetaStorePath;
        std::unordered_map<sd_gui_utils::DirTypes, unsigned int> ModelCount;

    public:
        Manager(const wxString& meta_base_path);
        ~Manager();
        sd_gui_utils::ModelFileInfo* addModel(wxString mpath, sd_gui_utils::DirTypes type, wxString name);
        bool exists(std::string model_path);
        std::string pathByName(std::string model_name);
        std::string getName(std::string model_path);
        void setHash(std::string model_path, std::string hash);
        sd_gui_utils::ModelFileInfo getByHash(std::string hash);
        sd_gui_utils::ModelFileInfo getInfo(std::string path);
        sd_gui_utils::ModelFileInfo* getIntoPtr(std::string path);
        sd_gui_utils::ModelFileInfo* getIntoPtrByHash(std::string hash);
        sd_gui_utils::ModelFileInfo getInfoByName(std::string model_name);
        sd_gui_utils::ModelFileInfo findInfoByName(std::string model_name);
        sd_gui_utils::ModelFileInfo* searchByName(const std::string &keyword, const sd_gui_utils::DirTypes &type);
        sd_gui_utils::ModelFileInfo* searchByName(const std::vector<std::string> &keywords, const sd_gui_utils::DirTypes &type);
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