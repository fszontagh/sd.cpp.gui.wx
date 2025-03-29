#ifndef LLVM_H
#define LLVM_H

namespace sd_gui_utils {

    enum llvmstatus {
        PENDING,
        MODEL_LOADED,
        SUCCESS,
        ERROR
    };

    enum llvmCommand {
        MODEL_LOAD,
        MODEL_UNLOAD,
        GENERATE_TEXT,
        GENERATE_TEXT_STREAM
    };

    enum llvmTextSender {
        USER,
        ASSISTANT,
        SYSTEM,
        SENDER_NONE
    };
    struct llvmText {
        llvmTextSender sender = SENDER_NONE;
        std::string text      = "";
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(llvmText, sender, text)

    class llvmMessage {
    private:
        uint64_t id                           = 0;
        uint64_t updated_at                   = 0;
        std::map<uint64_t, llvmText> messages = {};
        std::string model_path                = "";
        std::string status_message            = "";
        std::string title                     = "";
        llvmstatus status                     = sd_gui_utils::llvmstatus::PENDING;
        llvmCommand command                   = llvmCommand::MODEL_LOAD;
        int ngl                               = 4096;
        int n_ctx                             = 0;  // 0 - load from model (trained batch size)
        int n_batch                           = 0;
        int n_threads                         = 0;

    public:
        void SetId() {
            this->id = GenerateId();
        }
        uint64_t GetId() {
            return this->id;
        }
        const uint64_t GetUpdatedAt() const {
            return this->updated_at;
        }
        bool CheckUpdatedAt(const uint64_t &updated_at) {
            return this->updated_at == updated_at;
        }
        void SetCommandType(llvmCommand cmd) {
            this->command = cmd;
        }
        llvmCommand GetCommandType() {
            return this->command;
        }
        void SetModelPath(const std::string& path) {
            this->model_path = path;
        }
        const std::string GetModelPath() {
            return this->model_path;
        }
        void SetStatus(llvmstatus status) {
            this->status = status;
        }
        llvmstatus GetStatus() {
            return this->status;
        }
        void SetTitle(const std::string& title) {
            this->title = title;
        }
        void SetNgl(int ngl) {
            this->ngl = ngl;
        }
        int GetNgl() {
            return this->ngl;
        }
        void SetNCtx(int n_ctx) {
            this->n_ctx = n_ctx;
        }
        int GetNctx() {
            return this->n_ctx;
        }
        void SetNThreads(int n_threads) {
            this->n_threads = n_threads;
        }
        int GetNThreads() {
            return this->n_threads;
        }
        void SetNBatch(int n_batch) {
            this->n_batch = n_batch;
        }
        int GetNBatch() {
            return this->n_batch;
        }
        void SetStatusMessage(const std::string& msg) {
            this->status_message = msg;
            this->updated_at     = this->GenerateId();
        }
        std::string GetStatusMessage() {
            return this->status_message;
        }

        // generate unix timestamp as id
        int GenerateId() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                .count();
        }
        const std::string GetLatestUserPrompt() {
            for (const auto& p : this->messages) {
                if (p.second.sender == llvmTextSender::USER) {
                    return p.second.text;
                }
            }
            return "";
        }

        void AppendOrCreateLastAssistantAnswer(const std::string& str) {
            if (this->messages.empty()) {
                return;
            }
            auto last = this->messages.rbegin();
            if (last->second.sender == llvmTextSender::ASSISTANT) {
                last->second.text += str;
            } else {
                this->messages[this->GenerateId()] = llvmText{llvmTextSender::ASSISTANT, str};
            }
            this->updated_at = this->GenerateId();
        }
        void AppendUserPrompt(const std::string& str) {
            this->messages[this->GenerateId()] = llvmText{llvmTextSender::USER, str};
            this->updated_at                   = this->GenerateId();
        }
        std::string toString() {
            try {
                nlohmann::json j = *this;
                return j.dump();
            } catch (const std::exception& e) {
                // return "Error converting to string: " + std::string(e.what());
                std::cerr << "Error converting to string: " << e.what() << __FILE__ << ": " << __LINE__ << std::endl;
                return "";
            }
        }
        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(llvmMessage, id, updated_at, messages, model_path, status_message, status, command, ngl, n_ctx, n_batch, n_threads);
    };

};

#endif  // LLVM_H