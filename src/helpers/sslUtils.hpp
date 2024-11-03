#ifndef SSLUTILS_H
#define SSLUTILS_H
#include <openssl/evp.h>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>

namespace sd_gui_utils {

    inline std::string sha256_file_openssl(const char* path, void* custom_pointer, void (*callback)(size_t, std::string, void* custom_pointer)) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return "";
        }

        std::streamsize bufferSize = 1 * 1024 * 1024;
        char* buffer               = new char[bufferSize];

        std::string hashResult;
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hashLength;
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        const EVP_MD* md  = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, NULL);
        callback(file.tellg(), hashResult, custom_pointer);

        while (file.good()) {
            file.read(buffer, bufferSize);
            std::streamsize bytesRead = file.gcount();
            EVP_DigestUpdate(mdctx, buffer, bytesRead);

            if (file.tellg() % (20 * 1024 * 1024) == 0) {
                callback(file.tellg(), "", custom_pointer);
            }
        }

        EVP_DigestFinal_ex(mdctx, hash, &hashLength);
        EVP_MD_CTX_free(mdctx);

        std::stringstream ss;
        for (unsigned int i = 0; i < hashLength; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        callback(std::filesystem::file_size(path), ss.str(), custom_pointer);

        delete[] buffer;
        return ss.str();
    }
}  // namespace sd_gui_utils
#endif  // SSLUTILS_H