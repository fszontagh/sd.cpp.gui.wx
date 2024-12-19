#ifndef __SERVER_CONFIG_H
#define __SERVER_CONFIG_H

#include <cstddef>
#include <cstdint>

#include "libs/json.hpp"

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

struct ServerConfig {
    std::string host        = "127.0.0.1";
    uint16_t port           = 8191;
    int max_clients         = 10;
    size_t max_request_size = 1024 * 1024 * 1024;
    std::string logfile;
    backend_type backend = backend_type::AVX;
    std::string authkey;
    std::string server_name           = "";  // optional server name to display in the gui
    unsigned int unauthorized_timeout = 1;   // disconnect clients after this many seconds if not authenticated
    std::string exprocess_binary_path;       // force to use this exprocess binary
};

inline void to_json(nlohmann ::json& nlohmann_json_j, const ServerConfig& nlohmann_json_t) {
    nlohmann_json_j["host"]                 = nlohmann_json_t.host;
    nlohmann_json_j["port"]                 = nlohmann_json_t.port;
    nlohmann_json_j["max_clients"]          = nlohmann_json_t.max_clients;
    nlohmann_json_j["max_request_size"]     = nlohmann_json_t.max_request_size;
    nlohmann_json_j["logfile"]              = nlohmann_json_t.logfile;
    nlohmann_json_j["backend"]              = backend_type_to_str.at(nlohmann_json_t.backend);
    nlohmann_json_j["authkey"]              = nlohmann_json_t.authkey;
    nlohmann_json_j["unauthorized_timeout"] = nlohmann_json_t.unauthorized_timeout;
    if (nlohmann_json_t.server_name.empty() == false) {
        nlohmann_json_j["server_name"] = nlohmann_json_t.server_name;
    }
    if (nlohmann_json_t.exprocess_binary_path.empty() == false) {
        nlohmann_json_j["exprocess_binary_path"] = nlohmann_json_t.exprocess_binary_path;
    }
}
inline void from_json(const nlohmann ::json& nlohmann_json_j, ServerConfig& nlohmann_json_t) {
    const ServerConfig nlohmann_json_default_obj{};
    nlohmann_json_t.host             = nlohmann_json_j.value("host", nlohmann_json_default_obj.host);
    nlohmann_json_t.port             = nlohmann_json_j.value("port", nlohmann_json_default_obj.port);
    nlohmann_json_t.max_clients      = nlohmann_json_j.value("max_clients", nlohmann_json_default_obj.max_clients);
    nlohmann_json_t.max_request_size = nlohmann_json_j.value("max_request_size", nlohmann_json_default_obj.max_request_size);
    nlohmann_json_t.logfile          = nlohmann_json_j.value("logfile", nlohmann_json_default_obj.logfile);

    std::string backend = nlohmann_json_j.value("backend", "avx");
    if (backend_str_to_type.find(backend) == backend_str_to_type.end()) {
        backend = "avx";
    }
    nlohmann_json_t.backend = backend_str_to_type.at(backend);
    nlohmann_json_t.authkey = nlohmann_json_j.value("authkey", nlohmann_json_default_obj.authkey);
    if (nlohmann_json_j.contains("server_name") && !nlohmann_json_j["server_name"].is_null()) {
        nlohmann_json_t.server_name = nlohmann_json_j.value("server_name", nlohmann_json_default_obj.server_name);
    }
    if (nlohmann_json_j.contains("exprocess_binary_path") && !nlohmann_json_j["exprocess_binary_path"].is_null()) {
        nlohmann_json_t.exprocess_binary_path = nlohmann_json_j.value("exprocess_binary_path", nlohmann_json_default_obj.exprocess_binary_path);
    }
    nlohmann_json_t.unauthorized_timeout = nlohmann_json_j.value("unauthorized_timeout", nlohmann_json_default_obj.unauthorized_timeout);
}
#endif  // __SERVER_CONFIG_H
