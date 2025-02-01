#ifndef _BASE64_H_
#define _BASE64_H_

#include <fstream>
#include <sstream>

namespace sd_gui_utils {

    // Base64 karakterkészlet
    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    // Kódolás segédfüggvények
    inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    // Base64 kódolás
    inline std::string Base64Encode(const unsigned char* bytes_to_encode, size_t in_len) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    // Base64 dekódolás
    inline std::vector<unsigned char> Base64Decode(const std::string& encoded_string) {
        int in_len = encoded_string.size();
        int i      = 0;
        int j      = 0;
        int in_    = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_];
            in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++)
                ret.push_back(char_array_3[j]);
        }

        return ret;
    }

    // Fájlok kezelése
    inline bool EncodeFileToBase64(const std::string& inputFilePath, std::string& outputBase64) {
        std::cout << "EncodeFileToBase64: " << inputFilePath << std::endl;
        std::ifstream file(inputFilePath, std::ios::binary);
        if (!file.is_open())
            return false;

        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string fileData = buffer.str();
        outputBase64         = Base64Encode(reinterpret_cast<const unsigned char*>(fileData.data()), fileData.size());
        return true;
    }

    inline bool DecodeBase64ToFile(const std::string& inputBase64, const std::string& outputFilePath) {
        std::cout << "DecodeBase64ToFile: " << inputBase64.length() << " -> " << outputFilePath << std::endl;
        auto decodedData = Base64Decode(inputBase64);
        std::ofstream file(outputFilePath, std::ios::binary);
        if (!file.is_open())
            return false;

        file.write(reinterpret_cast<const char*>(decodedData.data()), decodedData.size());
        return true;
    }

    static inline bool StoreBase64ToFile(const std::string& inputFilePath, const std::string& targetMetaFilename) {
        std::cout << "StoreBase64ToFile: " << inputFilePath << " -> " << targetMetaFilename << std::endl;
        std::string inputBase64;
        if (!EncodeFileToBase64(inputFilePath, inputBase64)) {
            return false;
        }

        std::ofstream file(targetMetaFilename, std::ios::binary);
        if (!file) {
            return false;
        }

        file << inputBase64;
        return !file.fail();
    }

    static inline std::string ReadBase64FromFile(const std::string& metaFileName, bool decodeBase64 = false) {
        std::cout << "ReadBase64FromFile: " << metaFileName << std::endl;
        std::ifstream file(metaFileName, std::ios::binary);
        if (!file) {
            return {};
        }

        std::string fileBase64Data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        if (decodeBase64) {
            auto decodedData = Base64Decode(fileBase64Data);
            return {reinterpret_cast<const char*>(decodedData.data()), decodedData.size()};
        }

        return fileBase64Data;
    }

}  // namespace sd_gui_utils

#endif  // _BASE64_H_
