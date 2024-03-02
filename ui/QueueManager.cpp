#include "QueueManager.h"

QM::QueueManager::QueueManager(wxEvtHandler *eventHandler, std::string jobsdir)
{
    // need to send events into the mainwindow by the threads...
    this->eventHandler = eventHandler;
    this->eventHandler->Bind(wxEVT_THREAD, &QueueManager::OnThreadMessage, this);
    this->jobsDir = jobsdir;
    this->QueueList = std::map<int, QM::QueueItem>();
    this->LoadJobListFromDir();
}

QM::QueueManager::~QueueManager()
{
}

int QM::QueueManager::AddItem(QM::QueueItem item, bool fromFile)
{
    if (item.id == 0)
    {
        item.id = this->GetAnId();
    }
    if (item.created_at == 0)
    {
        item.created_at = this->GetCurrentUnixTimestamp();
    }

    this->QueueList[item.id] = item;

    this->SendEventToMainWindow(QM::QueueEvents::ITEM_ADDED, item);
    if (this->isRunning == false && item.status == QM::QueueStatus::PENDING)
    {
        this->SendEventToMainWindow(QM::QueueEvents::ITEM_START, item);
        this->isRunning = true;
    }
    if (!fromFile)
    {
        this->SaveJobToFile(item);
    }

    return item.id;
}

int QM::QueueManager::AddItem(sd_gui_utils::SDParams *params, bool fromFile)
{
    QM::QueueItem item;
    item.params = *params;
    return this->AddItem(item);
}

int QM::QueueManager::AddItem(sd_gui_utils::SDParams params, bool fromFile)
{
    QM::QueueItem item;
    item.params = params;
    return this->AddItem(item);
}

QM::QueueItem QM::QueueManager::GetItem(int id)
{
    if (this->QueueList.find(id) == this->QueueList.end())
    {
        return QM::QueueItem();
    }
    else
    {
        return this->QueueList[id];
    }
}

QM::QueueItem QM::QueueManager::GetItem(QM::QueueItem item)
{
    return this->GetItem(item.id);
}

const std::map<int, QM::QueueItem> QM::QueueManager::getList()
{
    return this->QueueList;
}

int QM::QueueManager::Duplicate(QM::QueueItem item)
{
    if (this->QueueList.find(item.id) == this->QueueList.end())
    {
        return -1;
    }
    else
    {
        auto origItem = this->QueueList.at(item.id);
        QM::QueueItem newitem;
        newitem.params = origItem.params;
        newitem.mode = origItem.mode;
        newitem.model = origItem.model;
        newitem.initial_image = origItem.initial_image;
        newitem.hash_fullsize = origItem.hash_fullsize;
        newitem.hash_progress_size = origItem.hash_progress_size;
        newitem.sha256 = origItem.sha256;
        return this->AddItem(newitem);
    }
}

int QM::QueueManager::Duplicate(int id)
{
    if (this->QueueList.find(id) != this->QueueList.end())
    {
        return this->Duplicate(this->QueueList[id]);
    }
    return -1;
}

void QM::QueueManager::SetStatus(QM::QueueStatus status, int id)
{
    if (this->QueueList.find(id) != this->QueueList.end())
    {
        this->QueueList[id].status = status;
        this->QueueList[id].updated_at = this->GetCurrentUnixTimestamp();
        if (status == QM::QueueStatus::DONE && this->QueueList[id].finished_at == 0)
        {
            this->QueueList[id].finished_at = this->GetCurrentUnixTimestamp();
        }
        this->SaveJobToFile(this->QueueList[id]);
        this->SendEventToMainWindow(QM::QueueEvents::ITEM_STATUS_CHANGED, this->QueueList[id]);
    }
}

void QM::QueueManager::SetStatus(QM::QueueStatus status, QM::QueueItem item)
{
    if (this->QueueList.find(item.id) != this->QueueList.end())
    {
        this->QueueList[item.id] = item;
    }
    this->SetStatus(status, item.id);
}

void QM::QueueManager::PauseAll()
{
    for (auto [key, value] : this->QueueList)
    {
        if (value.status == QM::QueueStatus::PENDING)
        {
            this->SetStatus(QM::PAUSED, key);
            this->SendEventToMainWindow(QM::QueueEvents::ITEM_STATUS_CHANGED);
        }
    }
}

void QM::QueueManager::SendEventToMainWindow(QM::QueueEvents eventType, QM::QueueItem item)
{
    // TODO: e->SetInt instead of SetString
    wxThreadEvent *e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%d",(int)sd_gui_utils::ThreadEvents::QUEUE, (int)eventType));
    e->SetPayload(item);
    wxQueueEvent(this->eventHandler, e);
}

void QM::QueueManager::OnThreadMessage(wxThreadEvent &e)
{

    if (e.GetSkipped() == false)
    {
        e.Skip();
    }
    auto msg = e.GetString().ToStdString();

    std::string token = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);
    // only numbers here...

    // only handle the QUEUE messages, what this class generate
    if (threadEvent == sd_gui_utils::QUEUE)
    {
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);
        auto payload = e.GetPayload<QM::QueueItem>();
        if (event == QM::QueueEvents::ITEM_START)
        {
            this->SetStatus(QM::QueueStatus::RUNNING, payload.id);
            this->isRunning = true;
            return;
        }
        if (event == QM::QueueEvents::ITEM_FINISHED)
        {
            this->SetStatus(QM::QueueStatus::DONE, payload);
            this->isRunning = false;
            // jump to the next item in queue
            // find waiting jobs
            for (auto job : this->QueueList)
            {
                if (job.second.status == QM::QueueStatus::PENDING)
                {
                    if (this->isRunning == false)
                    {
                        this->SendEventToMainWindow(QM::QueueEvents::ITEM_START, job.second);
                        this->isRunning = true;
                    }
                    break;
                }
            }
            return;
        }
        if (event == QM::QueueEvents::ITEM_MODEL_LOAD_START)
        {
            auto payload = e.GetPayload<QM::QueueItem>();
            this->SetStatus(QM::QueueStatus::MODEL_LOADING, payload.id);
        }
        if (event == QM::QueueEvents::ITEM_MODEL_FAILED)
        {
            auto payload = e.GetPayload<QM::QueueItem>();
            this->SetStatus(QM::QueueStatus::FAILED, payload.id);
            this->isRunning = false;
            // jump to the next
            for (auto job : this->QueueList)
            {
                if (job.second.status == QM::QueueStatus::PENDING)
                {
                    if (this->isRunning == false)
                    {
                        this->SendEventToMainWindow(QM::QueueEvents::ITEM_START, job.second);
                        this->isRunning = true;
                    }
                    break;
                }
            }
        }
        if (event == QM::QueueEvents::ITEM_FAILED)
        {
            auto payload = e.GetPayload<QM::QueueItem>();
            this->SetStatus(QM::QueueStatus::FAILED, payload.id);
            this->isRunning = false;
            // jump to the next
            for (auto job : this->QueueList)
            {
                if (job.second.status == QM::QueueStatus::PENDING)
                {
                    if (this->isRunning == false)
                    {
                        this->SendEventToMainWindow(QM::QueueEvents::ITEM_START, job.second);
                        this->isRunning = true;
                    }
                    break;
                }
            }
        }
        if (event == QM::QueueEvents::ITEM_GENERATION_STARTED)
        {
            auto payload = e.GetPayload<QM::QueueItem>();
            this->SetStatus(QM::QueueStatus::RUNNING, payload.id);
            this->isRunning = true;
        }
    }

    if (threadEvent == sd_gui_utils::ThreadEvents::HASHING_PROGRESS)
    {
        auto payload = e.GetPayload<QM::QueueItem>();
        this->SetStatus(QM::QueueStatus::HASHING, payload.id);
    }
}

void QM::QueueManager::SaveJobToFile(int id)
{
    auto item = this->GetItem(id);
    this->SaveJobToFile(item);
}

void QM::QueueManager::SaveJobToFile(QM::QueueItem item)
{
    try
    {
        nlohmann::json jsonfile(item);
        std::string filename = this->jobsDir + "/" + std::to_string(item.id) + ".json";
        std::ofstream file(filename);
        file << jsonfile;
        file.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

bool QM::QueueManager::DeleteJob(QM::QueueItem item)
{
    return this->DeleteJob(item.id);
}

bool QM::QueueManager::DeleteJob(int id)
{
    auto item = this->GetItem(id);
    if (item.id == 0)
    {
        return false;
    }
    std::string filename = this->jobsDir + "/" + std::to_string(item.id) + ".json";
    if (std::filesystem::exists(filename))
    {
        if (std::filesystem::remove(filename))
        {
            this->QueueList.erase(item.id);
            return true;
        }
    }
    return false;
}

int QM::QueueManager::GetCurrentUnixTimestamp()
{
    const auto p1 = std::chrono::system_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count());
}

void QM::QueueManager::LoadJobListFromDir()
{

    if (!std::filesystem::exists(this->jobsDir))
    {
        std::filesystem::create_directories(this->jobsDir);
    }

    int i = 0;
    for (auto const &dir_entry : std::filesystem::recursive_directory_iterator(this->jobsDir))
    {
        if (!dir_entry.exists() || !dir_entry.is_regular_file() || !dir_entry.path().has_extension())
        {
            continue;
        }

        std::filesystem::path path = dir_entry.path();

        std::string ext = path.extension().string();

        if (ext != ".json")
        {
            continue;
        }

        std::string name = path.filename().replace_extension("").string();

        std::ifstream f(path.string());
        nlohmann::json data;
        try
        {
            data = nlohmann::json::parse(f);
            auto item = data.get<QM::QueueItem>();
            if (item.status == QM::QueueStatus::RUNNING)
            {
                item.status = QM::QueueStatus::FAILED;
            }
            if (item.status == QM::QueueStatus::MODEL_LOADING)
            {
                item.status = QM::QueueStatus::PENDING;
            }
            this->AddItem(item, true);
        }
        catch (nlohmann::json::parse_error &ex)
        {
            std::cerr << "parse error at byte " << ex.byte << " in file: " << path.string() << std::endl;
        }
    }
}

int QM::QueueManager::GetAnId()
{
    int id = this->GetCurrentUnixTimestamp();
    while (id <= this->lastId)
    {
        id++;
    }
    this->lastId = id;
    return id;
}

void QM::QueueManager::onItemAdded(QM::QueueItem item)
{
    // this->parent->m_joblist
    // auto dataTable = this->parent->m_joblist;
}
