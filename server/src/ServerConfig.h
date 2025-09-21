#ifndef __SERVER_CONFIG_H
#define __SERVER_CONFIG_H

#include "wx/filename.h"
enum class backend_type {
    AVX,
    AVX2,
    AVX512,
    CUDA,
    HIPBLAS,
    VULKAN
};

inline const std::map<std::string, backend_type> backend_str_to_type = {
    {"avx", backend_type::AVX},
    {"avx2", backend_type::AVX2},
    {"avx512", backend_type::AVX512},
    {"cuda", backend_type::CUDA},
    {"hipblas", backend_type::HIPBLAS},
    {"vulkan", backend_type::VULKAN}};

inline const std::map<backend_type, std::string> backend_type_to_str = {
    {backend_type::AVX, "avx"},
    {backend_type::AVX2, "avx2"},
    {backend_type::AVX512, "avx512"},
    {backend_type::CUDA, "cuda"},
    {backend_type::HIPBLAS, "hipblas"},
    {backend_type::VULKAN, "vulkan"}};

struct ModelPaths {
    std::string checkpoints = "";
    std::string lora        = "";
    std::string vae         = "";
    std::string embedding   = "";
    std::string taesd       = "";
    std::string controlnet  = "";
    std::string esrgan      = "";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ModelPaths, checkpoints, lora, vae, embedding, taesd, controlnet, esrgan)

struct ServerConfig {
    std::string host           = "127.0.0.1";
    uint16_t port              = 8191;
    int max_clients            = 10;
    size_t max_request_size    = 1024 * 1024 * 1024;
    unsigned int tcp_keepalive = 30;  // keep alive period
    unsigned int cpu_cores     = 1;
    std::string logfile;
    std::string diffuser_logfile;
    backend_type backend = backend_type::AVX;
    std::string authkey;
    std::string shared_memory_path    = "";
    std::string server_name           = "";  // optional server name to display in the gui
    unsigned int unauthorized_timeout = 4;   // disconnect clients after this many seconds if not authenticated
    std::string exprocess_binary_path;       // force to use this exprocess binary
    ModelPaths model_paths = {};
    std::string server_id;
    std::string data_path              = "";     // where to store client info, jobs and images
    bool rewrite_server_id_in_jobfiles = false;  // ignore if the backreaded jobs don't match the server id !! WARN: this is dangerous, in normal case, this job shouldn't be loaded
                                                 // if true, the job will be loaded in and the server_id will be rewritten in the jobfile
                                                 // if false, the job will not loaded in

    /// @brief Retrieves the absolute path to the server data directory.
    /// @return A string representing the absolute path to the directory where client info, jobs, and images are stored.
    const std::string GetServerDataPath() {
        wxString path = this->data_path;
        path.Append(wxFileName::GetPathSeparators());
        auto p = wxFileName(path);
        return p.GetAbsolutePath().ToStdString();
    }
    const std::string GetJobsPath() {
        const auto path = wxString::Format("{}{}jobs{}", this->data_path, wxFileName::GetPathSeparator(), "jobs", wxFileName::GetPathSeparator());
        auto p          = wxFileName(path);
        return p.GetAbsolutePath().ToStdString();
    }
    const std::string GetClientDataPath() {
        const auto path = wxString::Format("{}{}clients{}", this->data_path, wxFileName::GetPathSeparator(), "clients", wxFileName::GetPathSeparator());
        auto p          = wxFileName(path);
        return p.GetAbsolutePath().ToStdString();
    }
};

inline void to_json(nlohmann ::json& nlohmann_json_j, const ServerConfig& nlohmann_json_t) {
    nlohmann_json_j["host"]                 = nlohmann_json_t.host;
    nlohmann_json_j["port"]                 = nlohmann_json_t.port;
    nlohmann_json_j["max_clients"]          = nlohmann_json_t.max_clients;
    nlohmann_json_j["max_request_size"]     = nlohmann_json_t.max_request_size;
    nlohmann_json_j["logfile"]              = nlohmann_json_t.logfile;
    nlohmann_json_j["diffuser_logfile"]     = nlohmann_json_t.diffuser_logfile;
    nlohmann_json_j["backend"]              = backend_type_to_str.at(nlohmann_json_t.backend);
    nlohmann_json_j["authkey"]              = nlohmann_json_t.authkey;
    nlohmann_json_j["unauthorized_timeout"] = nlohmann_json_t.unauthorized_timeout;
    nlohmann_json_j["server_id"]            = nlohmann_json_t.server_id;
    nlohmann_json_j["data_path"]            = nlohmann_json_t.data_path;
    if (nlohmann_json_t.shared_memory_path.empty() == false) {
        nlohmann_json_j["shared_memory_path"] = nlohmann_json_t.shared_memory_path;
    }
    if (nlohmann_json_t.server_name.empty() == false) {
        nlohmann_json_j["server_name"] = nlohmann_json_t.server_name;
    }
    if (nlohmann_json_t.exprocess_binary_path.empty() == false) {
        nlohmann_json_j["exprocess_binary_path"] = nlohmann_json_t.exprocess_binary_path;
    }
    if (nlohmann_json_t.rewrite_server_id_in_jobfiles) {
        nlohmann_json_j["rewrite_server_id_in_jobfiles"] = nlohmann_json_t.rewrite_server_id_in_jobfiles;
    }

    nlohmann_json_j["model_paths"] = nlohmann_json_t.model_paths;
}

inline void from_json(const nlohmann ::json& nlohmann_json_j, ServerConfig& nlohmann_json_t) {
    const ServerConfig nlohmann_json_default_obj{};
    nlohmann_json_t.host             = nlohmann_json_j.value("host", nlohmann_json_default_obj.host);
    nlohmann_json_t.port             = nlohmann_json_j.value("port", nlohmann_json_default_obj.port);
    nlohmann_json_t.max_clients      = nlohmann_json_j.value("max_clients", nlohmann_json_default_obj.max_clients);
    nlohmann_json_t.max_request_size = nlohmann_json_j.value("max_request_size", nlohmann_json_default_obj.max_request_size);
    nlohmann_json_t.logfile          = nlohmann_json_j.value("logfile", nlohmann_json_default_obj.logfile);
    nlohmann_json_t.diffuser_logfile = nlohmann_json_j.value("diffuser_logfile", nlohmann_json_default_obj.diffuser_logfile);
    nlohmann_json_t.server_id        = nlohmann_json_j.value("server_id", nlohmann_json_default_obj.server_id);
    nlohmann_json_t.data_path        = nlohmann_json_j.value("data_path", nlohmann_json_default_obj.data_path);

    std::string backend = nlohmann_json_j.value("backend", "avx");
    if (backend_str_to_type.find(backend) == backend_str_to_type.end()) {
        backend = "avx";
    }

    nlohmann_json_t.shared_memory_path = nlohmann_json_j.value("shared_memory_path", nlohmann_json_default_obj.shared_memory_path);
    nlohmann_json_t.backend            = backend_str_to_type.at(backend);
    nlohmann_json_t.authkey            = nlohmann_json_j.value("authkey", nlohmann_json_default_obj.authkey);
    if (nlohmann_json_j.contains("server_name") && !nlohmann_json_j["server_name"].is_null()) {
        nlohmann_json_t.server_name = nlohmann_json_j.value("server_name", nlohmann_json_default_obj.server_name);
    }
    if (nlohmann_json_j.contains("exprocess_binary_path") && !nlohmann_json_j["exprocess_binary_path"].is_null()) {
        nlohmann_json_t.exprocess_binary_path = nlohmann_json_j.value("exprocess_binary_path", nlohmann_json_default_obj.exprocess_binary_path);
    }
    nlohmann_json_t.unauthorized_timeout = nlohmann_json_j.value("unauthorized_timeout", nlohmann_json_default_obj.unauthorized_timeout);
    nlohmann_json_t.model_paths          = nlohmann_json_j.at("model_paths");

    if (nlohmann_json_j.contains("rewrite_server_id_in_jobfiles") && !nlohmann_json_j["rewrite_server_id_in_jobfiles"].is_null()) {
        nlohmann_json_t.rewrite_server_id_in_jobfiles = nlohmann_json_j.value("rewrite_server_id_in_jobfiles", nlohmann_json_default_obj.rewrite_server_id_in_jobfiles);
    }
}
#endif  // __SERVER_CONFIG_H
