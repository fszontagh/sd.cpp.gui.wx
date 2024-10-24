#ifndef SIMPLECURL_H
#define SIMPLECURL_H
#include <curl/curl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace sd_gui_utils {

    class SimpleCurl {
    public:
        SimpleCurl() {
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();
        }

        ~SimpleCurl() {
            if (curl) {
                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();
        }

        // Perform an HTTP GET request to download a file
        bool getFile(const std::string& url, const std::vector<std::string>& headers, const std::string& outputFilePath, std::function<void(size_t, size_t)> progressCallback = nullptr) {
            if (!curl) {
                return false;
            }

            // Open the file for writing
            std::ofstream outFile(outputFilePath, std::ios::binary);
            if (!outFile) {
                std::cerr << "Failed to open file for writing: " << outputFilePath << std::endl;
                return false;
            }

            // Set the URL
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 1L means true

            // Set the write callback to write directly to the file
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &SimpleCurl::writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);  // Write to file

            // Set the custom headers
            struct curl_slist* header_list = nullptr;
            for (const std::string& header : headers) {
                header_list = curl_slist_append(header_list, header.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            // Progress callback handling
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            if (progressCallback) {
                this->progressCallback_ = progressCallback;
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallbackStatic);
                curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
            }

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            // Check for errors
            if (res != CURLE_OK) {
                std::cerr << "Error downloading file: " << curl_easy_strerror(res) << std::endl;
                outFile.close();
                return false;
            }

            // Close the output file
            outFile.close();

            // Clean up the custom headers
            if (header_list) {
                curl_slist_free_all(header_list);
            }

            return true;
        }

        // Perform an HTTP GET request and store the response in a string
        bool get(const std::string& url, const std::vector<std::string>& headers, std::string& response, std::function<void(size_t, size_t)> progressCallback = nullptr) {
            if (!curl) {
                return false;
            }

            // Clear the response string
            response.clear();

            // Set the URL
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 1L means true

            // Set the write callback to collect the response data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &SimpleCurl::writeCallbackToString);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);  // Write to response string

            // Set the custom headers
            struct curl_slist* header_list = nullptr;
            for (const std::string& header : headers) {
                header_list = curl_slist_append(header_list, header.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

            // Progress callback handling
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
            if (progressCallback) {
                this->progressCallback_ = progressCallback;
                curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
                curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progressCallbackStatic);
                curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
            }

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            // Clean up the custom headers
            if (header_list) {
                curl_slist_free_all(header_list);
            }

            if (progressCallback != nullptr) {
                progressCallback = nullptr;
            }
            return res == CURLE_OK;
        }

    private:
        CURL* curl;

        // Static write callback to write the response data directly to the file
        static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
            std::ofstream* outFile = static_cast<std::ofstream*>(userdata);
            size_t totalSize       = size * nmemb;
            outFile->write(ptr, totalSize);  // Write the received data directly to the file
            return totalSize;
        }

        // Static write callback to collect the response data in a string
        static size_t writeCallbackToString(char* ptr, size_t size, size_t nmemb, void* userdata) {
            std::string* response = static_cast<std::string*>(userdata);
            size_t totalSize      = size * nmemb;
            response->append(ptr, totalSize);  // Append the received data to the response string
            return totalSize;
        }

        // Static progress callback wrapper
        static int progressCallbackWrapper(CURL* /*curl*/, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow, void* userdata) {
            auto progressCallback = static_cast<std::function<void(size_t, size_t)>*>(userdata);
            if (progressCallback) {
                // Call progress callback with current progress
                (*progressCallback)(static_cast<size_t>(dlnow), static_cast<size_t>(dltotal));
            }
            return 0;  // Return 0 to continue
        }

        static int progressCallbackStatic(void* p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
            auto simpleCurl = static_cast<SimpleCurl*>(p);
            if (simpleCurl->progressCallback_) {
                simpleCurl->progressCallback_(dlnow, dltotal);
            }
            return 0;
        }

        std::function<void(curl_off_t, curl_off_t)> progressCallback_;
    };

}  // namespace sd_gui_utils

#endif  // SIMPLECURL_H