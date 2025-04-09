#ifndef LLVM_H
#define LLVM_H

#ifdef stablediffusiongui_llama
#include "llama-cpp.h"
typedef std::vector<llama_chat_message> llama_message_list;
#endif
#include "../libs/chat-template.hpp"

namespace sd_gui_utils {

    enum class llvmstatus {
        LLVM_STATUS_PENDING,
        LLVM_STATUS_SUCCESS,
        LLVM_STATUS_ERROR
    };
    enum class llvmModelStatus {
        LLVM_MODEL_STATUS_LOADING,
        LLVM_MODEL_STATUS_LOADED,
        LLVM_MODEL_STATUS_UNLOADED
    };

    enum class llvmContextStatus {
        LLVM_CONTEXT_STATUS_LOADING,
        LLVM_CONTEXT_STATUS_LOADED,
        LLVM_CONTEXT_STATUS_UNLOADED
    };

    enum class llvmCommand {
        LLVM_COMMAND_MODEL_LOAD,
        LLVM_COMMAND_MODEL_UNLOAD,
        LLVM_COMMAND_GENERATE_TEXT,
        LLVM_COMMAND_GENERATE_TEXT_STREAM,
        LLVM_COMMAND_CTX_LOAD,
        LLVM_COMMAND_CTX_UNLOAD
    };

    enum class llvmTextSender {
        LLVM_TEXT_SENDER_USER,
        LLVM_TEXT_SENDER_ASSISTANT,
        LLVM_TEXT_SENDER_SYSTEM,
        LLVM_TEXT_SENDER_SENDER_NONE
    };
    inline const std::unordered_map<sd_gui_utils::llvmTextSender, std::string> llvmTextSenderMap = {
        {sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_USER, "user"},
        {sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_ASSISTANT, "assistant"},
        {sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_SYSTEM, "system"},
        {sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_SENDER_NONE, "none"}};

    inline const std::unordered_map<std::string, sd_gui_utils::llvmTextSender> llvmTextSenderMapReverse = {
        {"user", sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_USER},
        {"assistant", sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_ASSISTANT},
        {"system", sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_SYSTEM},
        {"none", sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_SENDER_NONE}};

    struct llvmText {
        llvmTextSender sender = sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_SENDER_NONE;
        std::string text      = "";
        uint64_t updated_at   = 0;
        uint64_t id           = 0;
        bool is_deleted       = false;
        size_t tokens_size    = 0;

        void UpdateText(const std::string& str) {
            this->text.append(str);
            this->updated_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch())
                                   .count();
        }
        void Delete(bool deleted = true) {
            this->is_deleted = deleted;
            this->updated_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::system_clock::now().time_since_epoch())
                                   .count();
        }
        void Update(const llvmText& t) {
            this->sender     = t.sender;
            this->text       = t.text;
            this->updated_at = t.updated_at;
            this->is_deleted = t.is_deleted;
        }
    };
    inline void to_json(nlohmann ::json& nlohmann_json_j, const llvmText& nlohmann_json_t) {
        nlohmann_json_j["sender"]     = sd_gui_utils::llvmTextSenderMap.at(nlohmann_json_t.sender);
        nlohmann_json_j["text"]       = nlohmann_json_t.text;
        nlohmann_json_j["updated_at"] = nlohmann_json_t.updated_at;
        nlohmann_json_j["id"]         = nlohmann_json_t.id;
        nlohmann_json_j["is_deleted"] = nlohmann_json_t.is_deleted;
    }
    inline void from_json(const nlohmann ::json& nlohmann_json_j, llvmText& nlohmann_json_t) {
        const llvmText nlohmann_json_default_obj{};
        std::string sender         = nlohmann_json_j.at("sender");
        nlohmann_json_t.sender     = sd_gui_utils::llvmTextSenderMapReverse.at(sender);
        nlohmann_json_t.text       = nlohmann_json_j.value("text", nlohmann_json_default_obj.text);
        nlohmann_json_t.updated_at = nlohmann_json_j.value("updated_at", nlohmann_json_default_obj.updated_at);
        nlohmann_json_t.id         = nlohmann_json_j.value("id", nlohmann_json_default_obj.id);
        nlohmann_json_t.is_deleted = nlohmann_json_j.value("is_deleted", nlohmann_json_default_obj.is_deleted);
    }
    struct llvmModel {
        sd_gui_utils::llvmModelStatus status    = sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_UNLOADED;
        std::string path                        = "";
        int ngl                                 = 4096;
        std::map<std::string, std::string> meta = {};
        std::string model_desc                  = "";
        uint64_t size                           = 0;
        std::string model_template              = "";
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(llvmModel, status, path, ngl, meta, model_desc, size, model_template)

    struct llvmContext {
        sd_gui_utils::llvmContextStatus status = sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED;
        int n_ctx                              = 0;  // 0 - load from model (trained batch size)
        int n_batch                            = 0;
        int n_threads                          = 0;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(llvmContext, status, n_ctx, n_batch, n_threads)

    /**
     * @brief Represents a message in an LLVM-based messaging system with support for chat-like interactions.
     *
     * This class manages message state, including text messages, model configuration,
     * command types, and status tracking. It provides methods for message manipulation,
     * model configuration, and status management.
     *
     * The class is designed to be move-constructible and move-assignable, but not copyable.
     * It uses a mutex for thread-safe operations and supports various LLVM-related commands
     * and statuses.
     */
    class llvmMessage {
    private:
        uint64_t id              = 0;
        uint64_t updated_at      = 0;
        uint64_t next_message_id = 1;

        std::map<uint64_t, sd_gui_utils::llvmText> messages;
        std::string status_message;
        std::string title;
        std::string prompt_template;

        sd_gui_utils::llvmstatus status   = sd_gui_utils::llvmstatus::LLVM_STATUS_PENDING;
        sd_gui_utils::llvmCommand command = llvmCommand::LLVM_COMMAND_MODEL_LOAD;
        sd_gui_utils::llvmModel model     = {};
        sd_gui_utils::llvmContext context;

        mutable std::mutex mutex;

    public:
        // Constructors and assignment operators
        llvmMessage()                                            = default;
        llvmMessage(const sd_gui_utils::llvmMessage&)            = delete;
        llvmMessage& operator=(const sd_gui_utils::llvmMessage&) = delete;

        // Move constructor
        llvmMessage(sd_gui_utils::llvmMessage&& other) noexcept
            : id(std::exchange(other.id, 0)), updated_at(std::exchange(other.updated_at, 0)), next_message_id(std::exchange(other.next_message_id, 1)), messages(std::move(other.messages)), model(std::move(other.model)), context(std::move(other.context)), status_message(std::move(other.status_message)), title(std::move(other.title)), prompt_template(std::move(other.prompt_template)), status(std::exchange(other.status, llvmstatus::LLVM_STATUS_PENDING)), command(std::exchange(other.command, llvmCommand::LLVM_COMMAND_MODEL_LOAD)), mutex() {}

        // Move assignment operator
        llvmMessage& operator=(sd_gui_utils::llvmMessage&& other) noexcept {
            if (this != &other) {
                id              = std::exchange(other.id, 0);
                updated_at      = std::exchange(other.updated_at, 0);
                next_message_id = std::exchange(other.next_message_id, 1);
                messages        = std::move(other.messages);
                model           = std::move(other.model);
                context         = std::move(other.context);
                status_message  = std::move(other.status_message);
                title           = std::move(other.title);
                prompt_template = std::move(other.prompt_template);
                status          = std::exchange(other.status, llvmstatus::LLVM_STATUS_PENDING);
                command         = std::exchange(other.command, llvmCommand::LLVM_COMMAND_MODEL_LOAD);
            }
            return *this;
        }

        // Message management
        void Update(const sd_gui_utils::llvmMessage& other);
        std::map<uint64_t, sd_gui_utils::llvmText> GetMessages(int first = -1, int last = -1);
        uint64_t InsertMessage(sd_gui_utils::llvmText& message, uint64_t id = 0);
        sd_gui_utils::llvmText GetMessage(uint64_t id);
        uint64_t GetNextMessageId();
        const sd_gui_utils::llvmText GetLatestMessage();
        uint64_t UpdateOrCreateAssistantAnswer(const std::string& str);
        uint64_t AppendUserPrompt(const std::string& str);
        const std::string GetLatestUserPrompt();

        // ID management
        void SetId();
        uint64_t GetId();
        [[nodiscard]]
        int GetCurrentUnixTimeStamp();
        const uint64_t GetUpdatedAt() const;
        bool CheckUpdatedAt(const uint64_t& updated_at);

        // Command
        void SetCommandType(sd_gui_utils::llvmCommand cmd);
        [[nodiscard]]
        sd_gui_utils::llvmCommand GetCommandType();
        [[nodiscard]]
        inline bool Command(const sd_gui_utils::llvmCommand& type) {
            return this->command == type;
        }

        // Status
        void SetStatus(sd_gui_utils::llvmstatus status);
        [[nodiscard]]
        sd_gui_utils::llvmstatus GetStatus();
        [[nodiscard]]
        inline bool Status(const sd_gui_utils::llvmstatus& status) {
            return this->status == status;
        }

        // Model status
        void SetStatus(const sd_gui_utils::llvmModelStatus& status);
        [[nodiscard]]
        const sd_gui_utils::llvmModelStatus GetModelStatus();
        [[nodiscard]]
        inline bool Status(const sd_gui_utils::llvmModelStatus& status) {
            return this->model.status == status;
        }

        // Context status
        void SetStatus(const sd_gui_utils::llvmContextStatus& status);
        [[nodiscard]]
        const sd_gui_utils::llvmContextStatus GetContextStatus();
        inline bool Status(const sd_gui_utils::llvmContextStatus& status) {
            return this->context.status == status;
        }

        // Model configuration
        void SetModelPath(const std::string& path);
        const std::string GetModelPath();
        void SetNgl(int ngl);
        int GetNgl();
        void SetNCtx(int n_ctx);
        int GetNctx();
        [[nodiscard]]
        inline int CalculateNCtx() {
            std::lock_guard<std::mutex> lock(mutex);

            if (this->model.meta.empty()) {
                return this->context.n_ctx;
            }

            if (this->model.meta.contains("general.architecture")) {
                const std::string architecture = this->model.meta["general.architecture"];
                if (this->model.meta.contains(architecture + ".context_length")) {
                    auto _n_ctx = std::stoi(this->model.meta[architecture + ".context_length"]);
                    if (_n_ctx > 0) {
                        return _n_ctx;
                    }
                }
            }
            return this->context.n_ctx;
        }

        void SetNThreads(int n_threads);
        int GetNThreads();
        void SetNBatch(int n_batch);
        int GetNBatch();
#ifdef stablediffusiongui_llama
        inline void SetModelMeta(const std::string& key, const std::string& val) {
            this->model.meta[key] = val;
            if (key == "tokenizer.chat_template") {
                this->model.model_template = val;
            }
            this->updated_at = this->GetCurrentUnixTimeStamp();
        }
        inline void ClearModelMeta() {
            this->model.meta.clear();
            this->updated_at = this->GetCurrentUnixTimeStamp();
        }
        [[nodiscard]]
        const std::string GetModelMeta(const std::string& key) {
            if (this->model.meta.find(key) == this->model.meta.end()) {
                return "";
            }
            return this->model.meta[key];
        }
        inline void RemoveMessages(std::vector<uint64_t> ids, std::function<bool(const sd_gui_utils::llvmText&)> callback = nullptr /* lambda callback*/) {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto& id : ids) {
                if (this->messages.find(id) == this->messages.end()) {
                    continue;
                }
                if (!callback) {
                    this->messages.at(id).Delete();
                }
                if (callback && callback(this->messages.at(id))) {
                    this->messages.at(id).Delete();
                }
            }
        }
#else
        [[nodiscard]]
        const std::map<wxString, wxString> GetModelMetaList() {
            std::map<wxString, wxString> list;
            for (auto& [key, val] : this->model.meta) {
                list[wxString(key.data(), key.size())] = wxString(val.data(), val.size());
            }
            return list;
        }
        [[nodiscard]]
        const wxString GetModelMeta(const std::string& key) {
            if (this->model.meta.find(key) == this->model.meta.end()) {
                return wxEmptyString;
            }
            return wxString(this->model.meta[key].data(), this->model.meta[key].size());
        }
#endif
        // Other settings
        void SetTitle(const std::string& title);
        void SetPromptTemplate(const std::string& str);
        std::string GetPromptTemplate();
        inline size_t FormatChatPrompt(const wxString& prompt_template, std::vector<char>& formatted, const std::string& system_prompt = "", bool add_ass = true) {
            for (const auto& msg : this->messages) {
            }
            return formatted.size();
        }

#ifdef stablediffusiongui_llama
        void SetStatusMessage(const std::string& msg);
        std::string GetStatusMessage();
        llama_message_list GetChatMessages();
#else
        void SetStatusMessage(const wxString& msg);
        wxString GetStatusMessage();
#endif

        std::string toString(const int indent = -1);
        [[nodiscard]]
        const inline nlohmann::json MessagesToJson() {
            nlohmann::json j = this->messages;
            return j;
        }
        inline const std::string MessagesFromNinja(const std::string& ninja_string, const std::string& bos_token = "", const std::string& eos_token = "") {
            if (ninja_string.empty()) {
                return "";
            }
            if (ninja_string.find("{%") != 0) {
                return "";
            }
            const auto jsonMessages = this->MessagesToJson();

            /* bos_token= */  // "<|start|>"
            /* eos_token= */  // "<|end|>"

            minja::chat_template tmpl(
                ninja_string,
                /* bos_token= */ bos_token,
                /* eos_token= */ eos_token);

            nlohmann::json formattedMessages = nlohmann::json::array();
            for (auto& [id, message] : jsonMessages.items()) {
                formattedMessages.push_back({{"role", message["sender"]},
                                             {"content", message["text"]}});
            }
            minja::chat_template_inputs inputs;
            inputs.messages              = formattedMessages;
            inputs.add_generation_prompt = true;

            /**
            inputs.tools = nlohmann::json::array({
                {
                    {"type", "function"},
                    {"function", {
                        {"name", "google_search"},
                        {"arguments", {{"query", "2+2"}}}
                    }}
                }
            });
              */

            return tmpl.apply(inputs);
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(sd_gui_utils::llvmMessage,
                                                    id,
                                                    updated_at,
                                                    messages,
                                                    model,
                                                    context,
                                                    status_message,
                                                    title,
                                                    status,
                                                    command,
                                                    next_message_id,
                                                    prompt_template);
    };
};
#endif  // LLVM_H