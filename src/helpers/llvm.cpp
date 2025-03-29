#include "llvm.h"

void sd_gui_utils::llvmMessage::Update(const llvmMessage& other) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->id != other.id) {
        throw std::runtime_error("Message IDs do not match");
    }
    this->updated_at      = other.updated_at;
    this->messages        = other.messages;
    this->model_path      = other.model_path;
    this->status_message  = other.status_message;
    this->title           = other.title;
    this->status          = other.status;
    this->command         = other.command;
    this->ngl             = other.ngl;
    this->n_ctx           = other.n_ctx;
    this->n_batch         = other.n_batch;
    this->n_threads       = other.n_threads;
    this->next_message_id = other.next_message_id;
}
std::map<uint64_t, sd_gui_utils::llvmText> sd_gui_utils::llvmMessage::GetMessages() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->messages;
}
void sd_gui_utils::llvmMessage::InsertMessage(const llvmText& message, uint64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    if (id == 0) {
        id = this->next_message_id++;
    }
    this->messages[id] = message;
}
sd_gui_utils::llvmText sd_gui_utils::llvmMessage::GetMessage(uint64_t id) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->messages.contains(id)) {
        return this->messages[id];
    }
    return llvmText();
}
uint64_t sd_gui_utils::llvmMessage::GetNextMessageId() {
    std::lock_guard<std::mutex> lock(mutex);
    return next_message_id;
}
void sd_gui_utils::llvmMessage::SetId() {
    std::lock_guard<std::mutex> lock(mutex);
    this->id = GenerateId();
}
uint64_t sd_gui_utils::llvmMessage::GetId() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->id;
}
const uint64_t sd_gui_utils::llvmMessage::GetUpdatedAt() const {
    std::lock_guard<std::mutex> lock(mutex);
    return this->updated_at;
}
bool sd_gui_utils::llvmMessage::CheckUpdatedAt(const uint64_t& updated_at) {
    std::lock_guard<std::mutex> lock(mutex);
    return this->updated_at == updated_at;
}
void sd_gui_utils::llvmMessage::SetCommandType(llvmCommand cmd) {
    std::lock_guard<std::mutex> lock(mutex);
    this->command = cmd;
}
sd_gui_utils::llvmCommand sd_gui_utils::llvmMessage::GetCommandType() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->command;
}
void sd_gui_utils::llvmMessage::SetModelPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex);
    this->model_path = path;
}
const std::string sd_gui_utils::llvmMessage::GetModelPath() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->model_path;
}
void sd_gui_utils::llvmMessage::SetStatus(llvmstatus status) {
    std::lock_guard<std::mutex> lock(mutex);
    this->status = status;
}
sd_gui_utils::llvmstatus sd_gui_utils::llvmMessage::GetStatus() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->status;
}
void sd_gui_utils::llvmMessage::SetTitle(const std::string& title) {
    std::lock_guard<std::mutex> lock(mutex);
    this->title = title;
}
void sd_gui_utils::llvmMessage::SetNgl(int ngl) {
    std::lock_guard<std::mutex> lock(mutex);
    this->ngl = ngl;
}
int sd_gui_utils::llvmMessage::GetNgl() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->ngl;
}
void sd_gui_utils::llvmMessage::SetNCtx(int n_ctx) {
    std::lock_guard<std::mutex> lock(mutex);
    this->n_ctx = n_ctx;
}
int sd_gui_utils::llvmMessage::GetNctx() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->n_ctx;
}
void sd_gui_utils::llvmMessage::SetNThreads(int n_threads) {
    std::lock_guard<std::mutex> lock(mutex);
    this->n_threads = n_threads;
}
int sd_gui_utils::llvmMessage::GetNThreads() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->n_threads;
}
void sd_gui_utils::llvmMessage::SetNBatch(int n_batch) {
    std::lock_guard<std::mutex> lock(mutex);
    this->n_batch = n_batch;
}
int sd_gui_utils::llvmMessage::GetNBatch() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->n_batch;
}
void sd_gui_utils::llvmMessage::SetStatusMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex);
    this->status_message = msg;
    this->updated_at     = this->GenerateId();
}
std::string sd_gui_utils::llvmMessage::GetStatusMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->status_message;
}
int sd_gui_utils::llvmMessage::GenerateId() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
const std::string sd_gui_utils::llvmMessage::GetLatestUserPrompt() {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& p : this->messages) {
        if (p.second.sender == llvmTextSender::USER) {
            return p.second.text;
        }
    }
    return "";
}
const sd_gui_utils::llvmText sd_gui_utils::llvmMessage::GetLatestMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->messages.empty()) {
        return llvmText();
    }
    auto last = this->messages.rbegin();
    return last->second;
}

void sd_gui_utils::llvmMessage::AppendUserPrompt(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex);
    auto t = llvmText{llvmTextSender::USER};
    t.UpdateText(str);
    this->messages[this->next_message_id++] = t;
    this->updated_at                        = this->GenerateId();
}

std::string sd_gui_utils::llvmMessage::toString() {
    std::lock_guard<std::mutex> lock(mutex);
    try {
        nlohmann::json j = *this;
        return j.dump();
    } catch (const std::exception& e) {
        std::cerr << "Error converting to string: " << e.what() << __FILE__ << ": " << __LINE__ << std::endl;
        return "";
    }
}

void sd_gui_utils::llvmMessage::UpdateOrCreateAssistantAnswer(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex);

    if (this->messages.empty()) {
        return;
    }

    auto last = this->messages.rbegin();
    if (last->second.sender == llvmTextSender::ASSISTANT) {
        last->second.UpdateText(str);
    } else {
        auto t = llvmText{llvmTextSender::ASSISTANT};
        t.UpdateText(str);
        this->messages[this->next_message_id++] = t;
    }

    this->updated_at = this->GenerateId();
};