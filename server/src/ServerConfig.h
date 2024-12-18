#ifndef __SERVER_CONFIG_H
#define __SERVER_CONFIG_H

struct ServerConfig {
    std::string host        = "127.0.0.1";
    uint16_t port           = 8191;
    int max_clients         = 10;
    size_t max_request_size = 1024 * 1024 * 1024;
    std::string logfile;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ServerConfig, host, port, max_clients, max_request_size, logfile)
#endif  // __SERVER_CONFIG_H
