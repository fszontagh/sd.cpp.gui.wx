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

    class llvmMessage {
    public:
        llvmMessage() {
            id = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
        }
        uint64_t id                = 0;
        std::string prompt        = "";
        std::string response       = "";
        std::string model_path     = "";
        std::string status_message = "";
        llvmstatus status          = sd_gui_utils::llvmstatus::PENDING;
        llvmCommand command        = llvmCommand::MODEL_LOAD;
        int ngl                    = 99;
        int n_ctx                  = 2048;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(llvmMessage, id, prompt, response, model_path, status_message, status, command, ngl, n_ctx);
};

#endif  // LLVM_H