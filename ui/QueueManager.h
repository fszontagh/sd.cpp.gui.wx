#ifndef __SD_GUI_QUEUE_MANAGER
#define __SD_GUI_QUEUE_MANAGER

#include <fstream>
#include <map>
#include <fmt/format.h>
#include <chrono>
#include <nlohmann/json.hpp>

#include "utils.hpp"

#include <wx/event.h>
#include <wx/window.h>

namespace QM
{
    enum QueueStatus
    {
        PENDING,
        RUNNING,
        PAUSED,
        FAILED,
        MODEL_LOADING,
        DONE,
        HASHING
    };

    enum GenerationMode
    {
        TXT2IMG,
        IMG2IMG,
        CONVERT,
        UPSCALE
    };

    inline const char *QueueStatus_str[] = {
        "pending",
        "running",
        "paused",
        "failed",
        "model loading...",
        "finished",
        "model hashing..."};

    enum QueueEvents
    {
        ITEM_DELETED,
        ITEM_ADDED,
        ITEM_STATUS_CHANGED,
        ITEM_UPDATED,
        ITEM_START,
        ITEM_FINISHED,
        ITEM_MODEL_LOAD_START,
        ITEM_MODEL_LOADED,
        ITEM_MODEL_FAILED,
        ITEM_GENERATION_STARTED,
        ITEM_FAILED,
        SD_MESSAGE
    };
    struct QueueItem
    {
        QueueItem() = default;
        QueueItem(const QueueItem &other)
            : id(other.id), created_at(other.created_at), updated_at(other.updated_at),
              finished_at(other.finished_at), params(other.params),
              status(other.status), images(other.images), step(other.step), steps(other.steps), hash_fullsize(other.hash_fullsize),
              hash_progress_size(other.hash_progress_size),
              time(other.time), model(other.model), mode(other.mode), initial_image(other.initial_image), status_message(other.status_message), upscale_factor(other.upscale_factor) {}

        QueueItem &operator=(const QueueItem &other)
        {
            if (this != &other)
            {
                id = other.id;
                created_at = other.created_at;
                updated_at = other.updated_at;
                finished_at = other.finished_at;
                images = other.images;
                params = other.params;
                status = other.status;
                step = other.step;
                steps = other.steps;
                time = other.time;
                model = other.model;
                mode = other.mode;
                initial_image = other.initial_image;
                hash_fullsize = other.hash_fullsize;
                hash_progress_size = other.hash_progress_size;
                status_message = other.status_message;
                upscale_factor = other.upscale_factor;
            }
            return *this;
        }
        int id = 0, created_at = 0, updated_at = 0, finished_at = 0;
        int step = 0, steps = 0;
        size_t hash_fullsize, hash_progress_size;
        float time = 0;
        uint32_t upscale_factor = 4;
        sd_gui_utils::SDParams params;
        QM::QueueStatus status = QM::QueueStatus::PENDING;
        std::vector<std::string> images;
        std::string model, initial_image;
        QM::GenerationMode mode = QM::GenerationMode::TXT2IMG;
        std::string sha256;
        std::string status_message = "";
    };

    inline void to_json(nlohmann::json &j, const QueueItem &p)
    {
        std::vector<std::string> _imgs;
        for (auto _img : p.images)
        {
            _imgs.insert(_imgs.begin(), sd_gui_utils::UnicodeToUTF8(_img));
        }
        j = nlohmann::json{
            {"id", p.id},
            {"created_at", p.created_at},
            {"updated_at", p.updated_at},
            {"finished_at", p.finished_at},
            {"images", _imgs},
            {"status", (int)p.status},
            {"model", sd_gui_utils::UnicodeToUTF8(p.model)},
            {"mode", (int)p.mode},
            {"params", p.params},
            {"upscale_factor", p.upscale_factor},
            {"initial_image", sd_gui_utils::UnicodeToUTF8(p.initial_image)},

        };
    }

    inline void from_json(const nlohmann::json &j, QueueItem &p)
    {
        j.at("id").get_to(p.id);
        j.at("created_at").get_to(p.created_at);
        j.at("updated_at").get_to(p.updated_at);
        if (j.contains("upscale_factor"))
        {
            j.at("upscale_factor").get_to(p.upscale_factor);
        }

        std::vector<std::string> _imgs = j.at("images").get<std::vector<std::string>>();
        for (auto _img : _imgs)
        {
            p.images.insert(p.images.begin(), sd_gui_utils::UTF8ToUnicode(_img));
        }

        j.at("finished_at").get_to(p.finished_at);
        j.at("model").get_to(p.model);
        j.at("params").get_to(p.params);
        j.at("initial_image").get_to(p.initial_image);
        p.status = j.at("status").get<QM::QueueStatus>();
        p.mode = j.at("mode").get<QM::GenerationMode>();
        p.initial_image = sd_gui_utils::UTF8ToUnicode(p.initial_image);
        p.model = sd_gui_utils::UTF8ToUnicode(p.model);
    }

    class QueueManager
    {
    public:
        QueueManager(wxEvtHandler *eventHandler, std::string jobsdir);
        ~QueueManager();
        int AddItem(QM::QueueItem item, bool fromFile = false);
        int AddItem(sd_gui_utils::SDParams *params, bool fromFile = false);
        int AddItem(sd_gui_utils::SDParams params, bool fromFile = false);
        QM::QueueItem GetItem(int id);
        QM::QueueItem GetItem(QM::QueueItem item);
        const std::map<int, QM::QueueItem> getList();
        int Duplicate(QM::QueueItem item);
        int Duplicate(int id);
        // @brief Update the status of an item by the item's id
        void SetStatus(QM::QueueStatus status, int id);
        // @brief Update the item too then update the status. This will store the list of the generated images too
        void SetStatus(QM::QueueStatus status, QM::QueueItem item);
        void PauseAll();
        void RestartQueue();
        void SendEventToMainWindow(QM::QueueEvents eventType, QM::QueueItem item = QM::QueueItem());
        void OnThreadMessage(wxThreadEvent &e);
        void SaveJobToFile(int id);
        void SaveJobToFile(QM::QueueItem item);
        bool DeleteJob(QM::QueueItem item);
        bool DeleteJob(int id);

    private:
        int GetCurrentUnixTimestamp();
        void LoadJobListFromDir();
        std::string jobsDir;
        int lastId = 0;
        int GetAnId();
        // thread events handler, toupdate main window data table
        void onItemAdded(QM::QueueItem item);

        // @brief check if something is running or not
        bool isRunning = false;

        wxEvtHandler *eventHandler;
        wxWindow *parent;
        std::map<int, QM::QueueItem> QueueList;
    };

};

#endif