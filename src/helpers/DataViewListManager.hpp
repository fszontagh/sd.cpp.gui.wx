#ifndef __SDGUI_HELPERS_DATAVIEWLISTMANAGER__
#define __SDGUI_HELPERS_DATAVIEWLISTMANAGER__

class DataViewListManager {
public:
    enum class queueJobColumns : unsigned int {
        ID             = 1 << 0,  // 1
        CREATED_AT     = 1 << 1,  // 2
        TYPE           = 1 << 2,  // 4
        SERVER         = 1 << 3,
        MODEL          = 1 << 4,   // 8
        SAMPLER        = 1 << 5,   // 16
        SEED           = 1 << 6,   // 32
        PROGRESS       = 1 << 7,   // 64
        SPEED          = 1 << 8,   // 128
        STATUS         = 1 << 9,   // 256
        STATUS_MESSAGE = 1 << 10,  // 512
        N_COUNTER      = 1 << 11   // 2048
    };
    inline static std::unordered_map<DataViewListManager::queueJobColumns, unsigned int> RowNumber = {
        {DataViewListManager::queueJobColumns::ID, 1},
        {DataViewListManager::queueJobColumns::CREATED_AT, 2},
        {DataViewListManager::queueJobColumns::TYPE, 3},
        {DataViewListManager::queueJobColumns::MODEL, 4},
        {DataViewListManager::queueJobColumns::SAMPLER, 5},
        {DataViewListManager::queueJobColumns::SEED, 6},
        {DataViewListManager::queueJobColumns::PROGRESS, 7},
        {DataViewListManager::queueJobColumns::SPEED, 8},
        {DataViewListManager::queueJobColumns::STATUS, 9},
        {DataViewListManager::queueJobColumns::STATUS_MESSAGE, 10},
        {DataViewListManager::queueJobColumns::SERVER, 11},
        {DataViewListManager::queueJobColumns::N_COUNTER, 12},

    };
    inline static unsigned int RowToInt(DataViewListManager::queueJobColumns type) {
        return DataViewListManager::RowNumber.at(type);
    }
    inline static bool hasFlag(DataViewListManager::queueJobColumns value, DataViewListManager::queueJobColumns flag) {
        return (static_cast<unsigned int>(value) & static_cast<unsigned int>(flag)) != 0;
    }

    DataViewListManager(wxDataViewListCtrl* parent, std::shared_ptr<QM::QueueManager> queueManager, sd_gui_utils::config* cfg)
        : parent(parent), store(parent->GetStore()), queueManager(queueManager), config(cfg) {}

    void DeleteItems(const wxDataViewItemArray& selected_items);
    void DeleteItems(std::vector<wxDataViewItem> items);
    void DeleteItem(uint64_t job_id);
    std::vector<uint64_t> GetSelectedItemsId(wxDataViewItemArray selected_items);
    std::shared_ptr<QueueItem> GetQueueItem(const unsigned int row);
    std::shared_ptr<QueueItem> GetQueueItem(const wxDataViewItem& item);
    std::vector<std::shared_ptr<QueueItem>> GetQueueItems(const wxDataViewItemArray& selected_items);
    bool ItemExists(uint64_t item_id);
    void AddItem(std::shared_ptr<QueueItem> item);
    void RemoveRemoteItems(const std::string& server_id);
    void UpdateColumn(DataViewListManager::queueJobColumns column, std::shared_ptr<QueueItem> item, std::function<wxVariant()> callback);
    void UpdateColumns(DataViewListManager::queueJobColumns columns, std::shared_ptr<QueueItem> item);

private:
    wxDataViewListCtrl* parent                     = nullptr;
    wxDataViewListStore* store                     = nullptr;
    std::shared_ptr<QM::QueueManager> queueManager = nullptr;
    sd_gui_utils::config* config                   = nullptr;
};

// Helper functions for bitwise operations
inline DataViewListManager::queueJobColumns operator|(DataViewListManager::queueJobColumns a, DataViewListManager::queueJobColumns b) {
    return static_cast<DataViewListManager::queueJobColumns>(
        static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline DataViewListManager::queueJobColumns operator&(DataViewListManager::queueJobColumns a, DataViewListManager::queueJobColumns b) {
    return static_cast<DataViewListManager::queueJobColumns>(
        static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline DataViewListManager::queueJobColumns operator~(DataViewListManager::queueJobColumns a) {
    return static_cast<DataViewListManager::queueJobColumns>(~static_cast<unsigned int>(a));
}

#endif  // __SDGUI_HELPERS_DATAVIEWLISTMANAGER__