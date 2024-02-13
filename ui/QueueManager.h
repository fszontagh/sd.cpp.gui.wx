#ifndef __SD_GUI_QUEUE_MANAGER
#define __SD_GUI_QUEUE_MANAGER

#include <fstream>
#include <map>
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
        DONE
    };

    enum GenerationMode
    {
        TXT2IMG,
        IMG2IMG,
        CONVERT
    };

    inline const char *QueueStatus_str[] = {
        "pending",
        "running",
        "paused",
        "failed",
        "model loading...",
        "finished"};

    enum QueueEvents
    {
        ITEM_DELETED,
        ITEM_ADDED,
        ITEM_STATUS_CHANGED,
        ITEM_UPDATED,
        ITEM_START,
        ITEM_FINISHED
    };
    struct QueueItem
    {
        QueueItem() = default;
        QueueItem(const QueueItem &other)
            : id(other.id), created_at(other.created_at), updated_at(other.updated_at),
              finished_at(other.finished_at), params(other.params), 
              status(other.status), images(other.images), step(other.step), steps(other.steps), 
              time(other.time), model(other.model), mode(other.mode), initial_image(other.initial_image) {}

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
            }
            return *this;
        }
        int id = 0, created_at = 0, updated_at = 0, finished_at = 0;
        int step = 0, steps = 0;
        float time = 0;
        sd_gui_utils::SDParams params;
        QM::QueueStatus status = QM::QueueStatus::PENDING;
        std::vector<std::string> images;
        std::string model, initial_image;
        QM::GenerationMode mode;
    };

    inline void to_json(nlohmann::json &j, const QueueItem &p)
    {
        j = nlohmann::json{
            {"id", p.id},
            {"created_at", p.created_at},
            {"updated_at", p.updated_at},
            {"finished_at", p.finished_at},
            {"images", p.images},
            {"status", (int)p.status},
            {"model", p.model},
            {"mode", (int)p.mode},
            {"params", p.params},
            {"initial_image", p.initial_image},

        };
    }

    inline void from_json(const nlohmann::json &j, QueueItem &p)
    {
        j.at("id").get_to(p.id);
        j.at("created_at").get_to(p.created_at);
        j.at("updated_at").get_to(p.updated_at);
        j.at("images").get_to(p.images);
        j.at("finished_at").get_to(p.finished_at);
        j.at("model").get_to(p.model);
        j.at("params").get_to(p.params);
        j.at("initial_image").get_to(p.initial_image);
        p.status = j.at("status").get<QM::QueueStatus>();
        p.mode = j.at("mode").get<QM::GenerationMode>();
    }

    class QueueManager
    {
    public:
        QueueManager(wxEvtHandler *eventHandler, std::string jobsdir);
        ~QueueManager();
        int AddItem(QM::QueueItem item);
        int AddItem(sd_gui_utils::SDParams *params);
        int AddItem(sd_gui_utils::SDParams params);
        QM::QueueItem GetItem(int id);
        QM::QueueItem GetItem(QM::QueueItem item);
        const std::map<int, QM::QueueItem> getList();
        int Duplicate(QM::QueueItem item);
        int Duplicate(int id);
        void SetStatus(QM::QueueStatus status, int id);
        void PauseAll();
        void SendEventToMainWindow(QM::QueueEvents eventType, QM::QueueItem item = QM::QueueItem());
        void OnThreadMessage(wxThreadEvent &e);
        void SaveJobToFile(int id);
        void SaveJobToFile(QM::QueueItem item);

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