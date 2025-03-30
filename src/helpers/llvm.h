#ifndef LLVM_H
#define LLVM_H

#ifdef stablediffusiongui_llama
#include "llama-cpp.h"
typedef std::vector<llama_chat_message> llama_message_list;
#endif

namespace sd_gui_utils {

    enum class llvmstatus {
        PENDING,
        MODEL_LOADED,
        MODEL_UNLOADED,
        SUCCESS,
        ERROR
    };

    enum class llvmCommand {
        MODEL_LOAD,
        MODEL_UNLOAD,
        GENERATE_TEXT,
        GENERATE_TEXT_STREAM
    };

    enum class llvmTextSender {
        USER,
        ASSISTANT,
        SYSTEM,
        SENDER_NONE
    };

    struct llvmText {
        llvmTextSender sender = sd_gui_utils::llvmTextSender::SENDER_NONE;
        std::string text      = "";
        uint64_t updated_at   = 0;

        void UpdateText(const std::string& str) {
            this->text.append(str);
            this->updated_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch())
                                   .count();
        }
        void Update(const llvmText& t) {
            this->sender     = t.sender;
            this->text       = t.text;
            this->updated_at = t.updated_at;
        }
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(llvmText, sender, text, updated_at)

    class llvmMessage {
    private:
        uint64_t id              = 0;
        uint64_t updated_at      = 0;
        uint64_t next_message_id = 1;

        std::map<uint64_t, sd_gui_utils::llvmText> messages;
        std::string model_path;
        std::string status_message;
        std::string title;
        std::string prompt_template;

        llvmstatus status   = sd_gui_utils::llvmstatus::PENDING;
        llvmCommand command = llvmCommand::MODEL_LOAD;

        int ngl       = 4096;
        int n_ctx     = 0;  // 0 - load from model (trained batch size)
        int n_batch   = 0;
        int n_threads = 0;

        mutable std::mutex mutex;

    public:
        // Constructors and assignment operators
        llvmMessage()                                            = default;
        llvmMessage(const sd_gui_utils::llvmMessage&)            = delete;
        llvmMessage& operator=(const sd_gui_utils::llvmMessage&) = delete;

        // Move constructor
        llvmMessage(sd_gui_utils::llvmMessage&& other) noexcept
            : id(std::exchange(other.id, 0)), updated_at(std::exchange(other.updated_at, 0)), next_message_id(std::exchange(other.next_message_id, 1)), messages(std::move(other.messages)), model_path(std::move(other.model_path)), status_message(std::move(other.status_message)), title(std::move(other.title)), prompt_template(std::move(other.prompt_template)), status(std::exchange(other.status, llvmstatus::PENDING)), command(std::exchange(other.command, llvmCommand::MODEL_LOAD)), ngl(std::exchange(other.ngl, 4096)), n_ctx(std::exchange(other.n_ctx, 0)), n_batch(std::exchange(other.n_batch, 0)), n_threads(std::exchange(other.n_threads, 0)), mutex() {}

        // Move assignment operator
        llvmMessage& operator=(sd_gui_utils::llvmMessage&& other) noexcept {
            if (this != &other) {
                id              = std::exchange(other.id, 0);
                updated_at      = std::exchange(other.updated_at, 0);
                next_message_id = std::exchange(other.next_message_id, 1);
                messages        = std::move(other.messages);
                model_path      = std::move(other.model_path);
                status_message  = std::move(other.status_message);
                title           = std::move(other.title);
                prompt_template = std::move(other.prompt_template);
                status          = std::exchange(other.status, llvmstatus::PENDING);
                command         = std::exchange(other.command, llvmCommand::MODEL_LOAD);
                ngl             = std::exchange(other.ngl, 4096);
                n_ctx           = std::exchange(other.n_ctx, 0);
                n_batch         = std::exchange(other.n_batch, 0);
                n_threads       = std::exchange(other.n_threads, 0);
            }
            return *this;
        }

        // Message management
        void Update(const sd_gui_utils::llvmMessage& other);
        std::map<uint64_t, sd_gui_utils::llvmText> GetMessages();
        void InsertMessage(const sd_gui_utils::llvmText& message, uint64_t id = 0);
        sd_gui_utils::llvmText GetMessage(uint64_t id);
        uint64_t GetNextMessageId();
        const sd_gui_utils::llvmText GetLatestMessage();
        void UpdateOrCreateAssistantAnswer(const std::string& str);
        void AppendUserPrompt(const std::string& str);
        const std::string GetLatestUserPrompt();

        // ID management
        void SetId();
        uint64_t GetId();
        int GenerateId();
        const uint64_t GetUpdatedAt() const;
        bool CheckUpdatedAt(const uint64_t& updated_at);

        // Command and status
        void SetCommandType(sd_gui_utils::llvmCommand cmd);
        sd_gui_utils::llvmCommand GetCommandType();
        void SetStatus(sd_gui_utils::llvmstatus status);
        sd_gui_utils::llvmstatus GetStatus();

        // Model configuration
        void SetModelPath(const std::string& path);
        const std::string GetModelPath();
        void SetNgl(int ngl);
        int GetNgl();
        void SetNCtx(int n_ctx);
        int GetNctx();
        void SetNThreads(int n_threads);
        int GetNThreads();
        void SetNBatch(int n_batch);
        int GetNBatch();

        // Other settings
        void SetTitle(const std::string& title);
        void SetPromptTemplate(const std::string& str);
        std::string GetPromptTemplate();

#ifdef stablediffusiongui_llama
        void SetStatusMessage(const std::string& msg);
        std::string GetStatusMessage();
        llama_message_list GetChatMessages();
#else
        void SetStatusMessage(const wxString& msg);
        wxString GetStatusMessage();
#endif

        std::string toString();

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(sd_gui_utils::llvmMessage,
                                                    id,
                                                    updated_at,
                                                    messages,
                                                    model_path,
                                                    status_message,
                                                    title,
                                                    status,
                                                    command,
                                                    ngl,
                                                    n_ctx,
                                                    n_batch,
                                                    n_threads,
                                                    next_message_id,
                                                    prompt_template);
    };

};
#endif  // LLVM_H