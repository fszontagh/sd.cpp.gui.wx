#include "llvm.h"

void sd_gui_utils::llvmMessage::Update(const llvmMessage& other) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->id != other.id) {
        throw std::runtime_error("Message IDs do not match");
    }
    this->updated_at      = other.updated_at;
    this->messages        = other.messages;
    this->model           = other.model;
    this->context         = other.context;
    this->status_message  = other.status_message;
    this->title           = other.title;
    this->status          = other.status;
    this->command         = other.command;
    this->next_message_id = other.next_message_id;
    this->prompt_template = other.prompt_template;
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
    this->id = GetCurrentUnixTimeStamp();
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
    this->model.path = path;
}
const std::string sd_gui_utils::llvmMessage::GetModelPath() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->model.path;
}
void sd_gui_utils::llvmMessage::SetStatus(sd_gui_utils::llvmstatus status) {
    std::lock_guard<std::mutex> lock(mutex);
    this->status     = status;
    this->updated_at = this->GetCurrentUnixTimeStamp();
}
sd_gui_utils::llvmstatus sd_gui_utils::llvmMessage::GetStatus() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->status;
}
void sd_gui_utils::llvmMessage::SetTitle(const std::string& title) {
    std::lock_guard<std::mutex> lock(mutex);
    this->title      = title;
    this->updated_at = this->GetCurrentUnixTimeStamp();
}
void sd_gui_utils::llvmMessage::SetNgl(int ngl) {
    std::lock_guard<std::mutex> lock(mutex);
    this->model.ngl  = ngl;
    this->updated_at = this->GetCurrentUnixTimeStamp();
}
int sd_gui_utils::llvmMessage::GetNgl() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->model.ngl;
}
void sd_gui_utils::llvmMessage::SetNCtx(int n_ctx) {
    std::lock_guard<std::mutex> lock(mutex);
    this->context.n_ctx = n_ctx;
    this->updated_at    = this->GetCurrentUnixTimeStamp();
}
int sd_gui_utils::llvmMessage::GetNctx() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->context.n_ctx;
}
void sd_gui_utils::llvmMessage::SetNThreads(int n_threads) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->context.n_threads == n_threads) {
        return;
    }
    this->context.n_threads = n_threads;
    this->updated_at        = this->GetCurrentUnixTimeStamp();
}
int sd_gui_utils::llvmMessage::GetNThreads() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->context.n_threads;
}
void sd_gui_utils::llvmMessage::SetNBatch(int n_batch) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->context.n_batch == n_batch) {
        return;
    }
    this->context.n_batch = n_batch;
    this->updated_at      = this->GetCurrentUnixTimeStamp();
}
int sd_gui_utils::llvmMessage::GetNBatch() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->context.n_batch;
}
#ifdef stablediffusiongui_llama
void sd_gui_utils::llvmMessage::SetStatusMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->status_message == msg)  {
        return;
    }
    this->status_message = msg;
    this->updated_at     = this->GetCurrentUnixTimeStamp();
}
std::string sd_gui_utils::llvmMessage::GetStatusMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->status_message;
}
#else
void sd_gui_utils::llvmMessage::SetStatusMessage(const wxString& msg) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->status_message == msg.ToStdString()) {
        return;
    }
    this->status_message = msg.ToStdString();
    this->updated_at     = this->GetCurrentUnixTimeStamp();
}
wxString sd_gui_utils::llvmMessage::GetStatusMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    return wxString(this->status_message.data(), wxConvUTF8);
}
#endif

int sd_gui_utils::llvmMessage::GetCurrentUnixTimeStamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}
const std::string sd_gui_utils::llvmMessage::GetLatestUserPrompt() {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->messages.empty()) {
        return "";
    }
    for (auto it = this->messages.rbegin(); it != this->messages.rend(); ++it) {
        if (it->second.sender == llvmTextSender::LLVM_TEXT_SENDER_USER) {
            return it->second.text;
        }
    }
    return "";
}
const sd_gui_utils::llvmText sd_gui_utils::llvmMessage::GetLatestMessage() {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->messages.empty()) {
        return llvmText();
    }
    return this->messages.rbegin()->second;
}

void sd_gui_utils::llvmMessage::AppendUserPrompt(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex);
    if (str.empty()) {
        return;
    }
    auto t = llvmText{llvmTextSender::LLVM_TEXT_SENDER_USER};
    t.UpdateText(str);
    this->messages[this->next_message_id++] = t;
    this->updated_at                        = this->GetCurrentUnixTimeStamp();
}

std::string sd_gui_utils::llvmMessage::toString(const int indent) {
    std::lock_guard<std::mutex> lock(mutex);
    try {
        nlohmann::json j = *this;
        return j.dump(indent);
    } catch (const std::exception& e) {
        std::cerr << "Error converting to string: " << e.what() << __FILE__ << ": " << __LINE__ << std::endl;
        return "";
    }
}

void sd_gui_utils::llvmMessage::UpdateOrCreateAssistantAnswer(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex);

    if (this->messages.empty() || str.empty()) {
        return;
    }

    auto last = this->messages.rbegin();
    if (last->second.sender == llvmTextSender::LLVM_TEXT_SENDER_ASSISTANT) {
        last->second.UpdateText(str);
    } else {
        auto t = llvmText{llvmTextSender::LLVM_TEXT_SENDER_ASSISTANT};
        t.UpdateText(str);
        this->messages[this->next_message_id++] = t;
    }

    this->updated_at = this->GetCurrentUnixTimeStamp();
};

void sd_gui_utils::llvmMessage::SetPromptTemplate(const std::string& pt) {
    std::lock_guard<std::mutex> lock(mutex);
    if (pt.empty()) {
        return;
    }
    if (this->prompt_template == pt) {
        return;
    }
    this->prompt_template = pt;
    this->updated_at      = this->GetCurrentUnixTimeStamp();
}
std::string sd_gui_utils::llvmMessage::GetPromptTemplate() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->prompt_template;
}
#ifdef stablediffusiongui_llama
llama_message_list sd_gui_utils::llvmMessage::GetChatMessages() {
    std::lock_guard<std::mutex> lock(mutex);

    llama_message_list messages;
    for (auto& [id, message] : this->messages) {
        llama_chat_message msg;
        msg.content = message.text.c_str();
        msg.role    = sd_gui_utils::llvmTextSenderMap.at(message.sender).c_str();
        messages.emplace_back(msg);
    }
    return messages;
};
#endif
void sd_gui_utils::llvmMessage::SetStatus(const sd_gui_utils::llvmModelStatus& status) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->model.status == status) {
        return;
    }
    this->model.status = status;
    this->updated_at   = this->GetCurrentUnixTimeStamp();
};
const sd_gui_utils::llvmModelStatus sd_gui_utils::llvmMessage::GetModelStatus() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->model.status;
};
const sd_gui_utils::llvmContextStatus sd_gui_utils::llvmMessage::GetContextStatus() {
    std::lock_guard<std::mutex> lock(mutex);
    return this->context.status;
};
void sd_gui_utils::llvmMessage::SetStatus(const sd_gui_utils::llvmContextStatus& status) {
    std::lock_guard<std::mutex> lock(mutex);
    if (this->context.status == status) {
        return;
    }
    this->context.status = status;
    this->updated_at     = this->GetCurrentUnixTimeStamp();
};