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

int QM::QueueManager::AddItem(QM::QueueItem item)
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
    this->SaveJobToFile(item);
    return item.id;
}

int QM::QueueManager::AddItem(sd_gui_utils::SDParams *params)
{
    QM::QueueItem item;
    item.params = *params;
    return this->AddItem(item);
}

int QM::QueueManager::AddItem(sd_gui_utils::SDParams params)
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
        return this->AddItem(this->QueueList.at(item.id).params);
    }
}

int QM::QueueManager::Duplicate(int id)
{
    return this->AddItem(this->GetItem(id).params);
}

void QM::QueueManager::SetStatus(QM::QueueStatus status, int id)
{
    if (this->QueueList.find(id) != this->QueueList.end())
    {
        this->QueueList[id].status = status;
        this->QueueList[id].updated_at = this->GetCurrentUnixTimestamp();
        if (status == QM::QueueStatus::DONE)
        {
            this->QueueList[id].finished_at = this->GetCurrentUnixTimestamp();
        }
        this->SaveJobToFile(this->QueueList[id]);
        this->SendEventToMainWindow(QM::QueueEvents::ITEM_STATUS_CHANGED, this->QueueList[id]);
    }
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
    wxThreadEvent *e = new wxThreadEvent();
    e->SetString(wxString::Format("QUEUE:%d", (int)eventType));
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
    // only numbers here...

    // only handle the QUEUE messages, what this class generate
    if (token == "QUEUE")
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
            this->SetStatus(QM::QueueStatus::DONE, payload.id);
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
    }

    if (token == "MODEL_LOAD_START")
    {
        auto payload = e.GetPayload<QM::QueueItem>();
        this->SetStatus(QM::QueueStatus::MODEL_LOADING, payload.id);
    }

    // this state can not usable at here, because the payload is the sd_ctx* pointer here..
    // we can't identify the current running job here... (we can maybe guess it, but not needed)
    // see  GENERATION_START
    if (token == "MODEL_LOAD_DONE")
    {
        // auto payload = e.GetPayload<QM::QueueItem>();
        // this->SetStatus(QM::QueueStatus::RUNNING, payload.id);
    }
    if (token == "MODEL_LOAD_ERROR" || token == "GENERATION_ERROR")
    {
        auto payload = e.GetPayload<QM::QueueItem>();
        this->SetStatus(QM::QueueStatus::FAILED, payload.id);
        this->isRunning = false;
    }
    if (token == "GENERATION_START")
    {
        auto payload = e.GetPayload<QM::QueueItem>();
        this->SetStatus(QM::QueueStatus::RUNNING, payload.id);
        this->isRunning = true;
    }
    // nothing to todo here, the payload is the generated image list, we can't find whitch item was it...
    // TODO: use struct to payload, and store multiple items in it...
    if (token == "GENERATION_DONE")
    {
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
        try
        {
            nlohmann::json data = nlohmann::json::parse(f);
            auto item = data.get<QM::QueueItem>();
            if (item.status == QM::QueueStatus::RUNNING)
            {
                item.status = QM::QueueStatus::FAILED;
            }
            if (item.status == QM::QueueStatus::MODEL_LOADING)
            {
                item.status = QM::QueueStatus::FAILED;
            }
            this->AddItem(item);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
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
